
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include "common.h"
#include "pinconfig.h"
#include "motor.h"
#include "spi.h"
#include "adc.h"
#include "control.h"


/* Controller Structures */
anklestate_t *s_ptr;
cntlr_flgs_t *flgs_ptr;
config_t *config_ptr;

/* ----------------------------------------------------------------------------
 * Function init_control(*,*) initializes control structures, control loop
 * and log file.
 *
 * Input: frq_hz -  control loop frequency
 *        f      -  string
 * Output: error | 0
 * ------------------------------------------------------------------------- */
int init_control(const float frq_hz, FILE* f)
{
  struct sigaction action_loop, action_cleanup;

  /* Allocate */
  s_ptr = malloc(sizeof(anklestate_t));
  flgs_ptr = malloc(sizeof(cntlr_flgs_t));
  config_ptr = malloc(sizeof(config_t));

  /* Initialize */
  config_ptr->codeVersion = 0;
  config_ptr->frq_hz = frq_hz;
  config_ptr->dt_us = (suseconds_t)(1000000.0 / config_ptr->frq_hz);

  /* Initialize state */
  s_ptr->time_stamp = 0.0;
  s_ptr->pos = 0.0;
  s_ptr->pos_0 = ANGLE_OFFSET;
  s_ptr->vel = 0.0;

  s_ptr->motor_cur = 0.0;
  s_ptr->motor_vel = 0.0;

  s_ptr->Kp = 100.0;
  s_ptr->Kd = 15.0;
  s_ptr->cmd = 0.0;

  s_ptr->FF_gain = 15.0;
  s_ptr->FF_period = 1.0;
  s_ptr->FF_phase = 0.0;

  for (int i=0; i<s_ptr->numOfADC; i++){
    s_ptr->adc_value[i] = 0.0;
    s_ptr->adc_ch[i] = 0;
  }

  flgs_ptr->control_loop_started = 0;
  flgs_ptr->FB_cntlr = 1;
  flgs_ptr->FF_cntlr = 0;

  if(create_log_file() != 0){
    printf("Cannot create log file.\n");
    control_cleanup(0);
    return -1;
  }

  if(init_motor() != 0){
    printf("Motor initialization error.\n");
    fprintf(config_ptr->f_log,"Motor initialization error.\n");
    control_cleanup(0);
    return -1;
  }

  if(init_spi() != 0){
    printf("SPI initialization error.\n");
    fprintf(config_ptr->f_log,"SPI initialization error.\n");
    control_cleanup(0);
    return -1;
  }

  if(init_adc() != 0){
    printf("ADC initialization error.\n");
    fprintf(config_ptr->f_log, "ADC initialization error.\n");
    control_cleanup(0);
  }

  /* set handler for control loop signal */
  action_loop.sa_handler = control_loop_cb;
  sigemptyset(&action_loop.sa_mask);
  action_loop.sa_flags = 0;
  if(sigaction(SIGALRM, &action_loop, NULL) != 0){
    printf("Unable to catch SIGALRM\n");
    fprintf(config_ptr->f_log, "Unable to catch SIGALRM\n");
    return -1;
  }

  /* Set handler for cleanup */
  action_cleanup.sa_handler = control_cleanup;
  sigemptyset(&action_cleanup.sa_mask);
  action_cleanup.sa_flags = 0;
  if (sigaction(SIGINT, &action_cleanup, NULL) != 0){
    printf("Unable to catch SIGINT\n");
    fprintf(config_ptr->f_log, "Unable to catch SIGINT\n");
    return -1;
  }

  return 0;
}


/* ----------------------------------------------------------------------------
 * Function start_control() starts alarm signal for controller call back.
 * ------------------------------------------------------------------------- */
int start_control(void)
{
  struct itimerval it_val;

  // Start alarm timer
  it_val.it_value.tv_sec = 0;
  it_val.it_value.tv_usec = config_ptr->dt_us;
  it_val.it_interval = it_val.it_value;
  if (setitimer(ITIMER_REAL, &it_val, NULL) == -1){
    printf("Error calling setitimer()\n");
    fprintf(config_ptr->f_log, "Error calling setitimer()\n");
    return -1;
  }

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function control_cleanup(*) is a callback to cleanup controller.
 * ------------------------------------------------------------------------- */
void control_cleanup(int signum)
{
  alarm(0);
  printf("\nCleaning up....\n");
  fclose(config_ptr->f_log);
  printf("Log closed.\n");
  spi_cleanup();
  motor_cleanup();
  exit(0);
}

/* ----------------------------------------------------------------------------
 * Function control_loop_cb(*) is a callback that runs control loop functions.
 * Inside control loop the following happens:
 *  (1) update states
 *  (2) update control
 *  (3) log data
 * ------------------------------------------------------------------------- */
void control_loop_cb(int signum)
{
  struct timeval t_now;
  struct timeval t_end;

  /* start time */
  if(gettimeofday(&t_now, NULL) == -1){
    printf("Error getting time.");
    fprintf(config_ptr->f_log, "Error getting time.\n");
  }

  /* check control loop flag, set time stamp, and flag */
  if (flgs_ptr->control_loop_started != 0){
    s_ptr->time_stamp = get_time_stamp(t_now);
  }
  else{
    s_ptr->time_stamp = 0.0;
    config_ptr->start_time = t_now;
    flgs_ptr->control_loop_started = 1;
  }

  /* State update */
  if (update_state() != 0){
    printf("State update failed\n");
    fprintf(config_ptr->f_log,"State update failed\n");
  }

  /* Control update */
  if (update_control() != 0){
    printf("Control update failed\n");
    fprintf(config_ptr->f_log,"Control update failed\n");
  }

  /* Get cpu time for control loop */
  if(gettimeofday(&t_end, NULL) == -1){
    printf("Error getting time.");
    fprintf(config_ptr->f_log, "Error getting time.\n");
  }

  /* Log data */
  fprintf(config_ptr->f_log,"%lf %f %f %f %f %f %f %f %f %f\n",
                      s_ptr->time_stamp,
                      get_time_interval(t_now, t_end),
                      s_ptr->pos,
                      s_ptr->pos_0,
                      s_ptr->vel,
                      s_ptr->motor_cur,
                      s_ptr->motor_vel,
                      s_ptr->cmd,
                      s_ptr->Kp,
                      s_ptr->Kd);
  if (debug){
    printf("%lf %f %f %f %f %f %f %f %f %f\n",
           s_ptr->time_stamp,
           get_time_interval(t_now, t_end),
           s_ptr->pos,
           s_ptr->pos_0,
           s_ptr->vel,
           s_ptr->motor_cur,
           s_ptr->motor_vel,
           s_ptr->cmd,
           s_ptr->Kp,
           s_ptr->Kd);
  }
}

/* ----------------------------------------------------------------------------
 * Function update_state() updates the controller states.
 *
 * TO DO: scale ADC appropriately
 * ------------------------------------------------------------------------- */
int update_state(void)
{
  float prev_pos_error = s_ptr->pos_0 - s_ptr->pos;

  /* ankle position */
//  if (read_pos() != 0){
//    printf("State update failed -- get_pos().\n");
//    fprintf(config_ptr->f_log,"State update failed -- get_pos().\n");
//    return -1;
//  }

  /* ankle velocity */
//  s_ptr->vel = (s_ptr->pos_0 - s_ptr->pos - prev_pos_error)*config_ptr->frq_hz;

  /* Read all adc channels */
  if (read_adc() != 0){
    printf("Error reading adc\n");
    return -1;
  }
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function update_control() updates control command
 * ------------------------------------------------------------------------- */
int update_control(void)
{
  s_ptr->cmd = 0;

  /* Feedback Controller */
  if(flgs_ptr->FB_cntlr)
    s_ptr->cmd = s_ptr->cmd +
                s_ptr->Kp*(s_ptr->pos_0 - s_ptr->pos) + s_ptr->Kd*s_ptr->vel;

  /* Feedforward Controller */
  if(flgs_ptr->FF_cntlr){

    float w = 2.0*M_PI*(1.0/s_ptr->FF_period);
    float t = s_ptr->time_stamp;
    float phi = s_ptr->FF_phase;

    s_ptr->cmd = s_ptr->cmd + s_ptr->FF_gain*(sin(w*t + phi));
  }

  /* Send cmd */
  if(set_motor_duty(s_ptr->cmd) !=0){
    printf("pwm update failed.\n");
    return -1;
  }

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function read_pos(*) is low level get ankle position using SPI.
 * ------------------------------------------------------------------------- */
int read_pos(void)
{
  uint8_t MSB[1] = {0x00};
  uint8_t LSB[1] = {0x00};

  /* send cmd */
  uint8_t cmd[1] = {0x10};
  uint8_t recv[ARRAY_SIZE(cmd)] = {0,};
  if(spi_transfer(cmd, recv) != 0){
    printf("spi transfer failed\n");
    return -1;
  }

  /* send 0x00 till acknowledgement */
  cmd[0] = 0x00;
  while(recv[0] != 0x10){
    if(spi_transfer(cmd, recv) != 0){
      printf("spi transfer failed\n");
      return -1;
    }
  }

  /* receive position */
  cmd[0] = 0x00;
  if(spi_transfer(cmd, MSB) != 0){
    printf("spi transfer failed\n");
    return -1;
  }
  cmd[0] = 0x00;
  if(spi_transfer(cmd, LSB) != 0){
    printf("spi transfer failed\n");
    return -1;
  }

  s_ptr->pos = encoder_to_angle(MSB[0],LSB[0]);
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function encoder_to_angle(*,*) converts encoder counts to radians.
 * ------------------------------------------------------------------------- */
float encoder_to_angle(uint8_t MSB, uint8_t LSB)
{
  float angle = ((float)((MSB << 8) | (LSB & 0xff)))*2.0*M_PI/4096.0;
  if(angle > M_PI){
    angle = angle - 2.0*M_PI;
  }
  return angle + ANGLE_OFFSET;
}

/* ----------------------------------------------------------------------------
 * Helper functions to allow acces to local state ptr.
 * ------------------------------------------------------------------------- */
int set_pos_0(float new_pos)
{
  s_ptr->pos_0 = new_pos;
  return 0;
}

int set_Kp(float new_Kp)
{
  s_ptr->Kp = new_Kp;
  return 0;
}

int set_Kd(float new_Kd)
{
  s_ptr->Kd = new_Kd;
  return 0;
}

float get_Kp(void)
{
  return s_ptr->Kp;
}

float get_Kd(void)
{
  return s_ptr->Kd;
}

float get_pos_0(void)
{
  return s_ptr->pos_0;
}

/* ----------------------------------------------------------------------------
 * Function create_log_file() creates log file based on data and time.
 * ------------------------------------------------------------------------- */
int create_log_file(void)
{
  char logstr[256] = "datalog/log_BB_";
  char timestr[256];

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  /* Create log file */
  strftime(timestr, sizeof(timestr)-1, "%Y_%m_%d_%H-%M", t);
  strcat(logstr, timestr);
  strcat(logstr, ".txt");
  config_ptr->f_log = fopen(logstr, "w");
  printf("Log file: %s\n",logstr);

  /* Create header */
  fprintf(config_ptr->f_log,"%% BBB ankleOS version %i\n",
         config_ptr->codeVersion);
  fprintf(config_ptr->f_log,"%% Date: %s\n", timestr);
  fprintf(config_ptr->f_log,"%%\n%% timestamp kp\n");
  fprintf(config_ptr->f_log,"%% Sample frequency = %f\n",config_ptr->frq_hz);

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function get_time_stamp(*) creates time stamp for data.
 * ------------------------------------------------------------------------- */
float get_time_stamp(struct timeval t_now)
{
  return((float)(t_now.tv_sec - config_ptr->start_time.tv_sec)
       + (float)(t_now.tv_usec - config_ptr->start_time.tv_usec)/1000000.0);
}

/* ----------------------------------------------------------------------------
 * Function get_time_interval(*,*) calculates interval.
 * ------------------------------------------------------------------------- */
float get_time_interval(struct timeval t1, struct timeval t2)
{
  return((float)(t2.tv_sec - t1.tv_sec)
       + (float)(t2.tv_usec - t1.tv_usec)/1000000.0);
}

/* ----------------------------------------------------------------------------
 * Function get_time_interval_us(*,*) calculates interval in us.
 * ------------------------------------------------------------------------- */
suseconds_t get_time_interval_us(struct timeval t1, struct timeval t2)
{
  return (t2.tv_usec - t1.tv_usec);
}

/* ----------------------------------------------------------------------------
 * Function set_FB_cntrl()
 * ------------------------------------------------------------------------- */
int set_FB_cntrl(int is_FB_on)
{
  if(is_FB_on && !flgs_ptr->FB_cntlr){
    flgs_ptr->FB_cntlr = 1;
    return 0;
  }
  else if (!is_FB_on && flgs_ptr->FB_cntlr){
    flgs_ptr->FB_cntlr = 0;
    return 0;
  }
  else
    return 0;
}

/* ----------------------------------------------------------------------------
 * Function set_FF_cntrl()
 * ------------------------------------------------------------------------- */
int set_FF_cntrl(int is_FF_on)
{
  if(is_FF_on && !flgs_ptr->FF_cntlr){
    flgs_ptr->FF_cntlr = 1;
    return 0;
  }
  else if (!is_FF_on && flgs_ptr->FF_cntlr){
    flgs_ptr->FF_cntlr = 0;
    return 0;
  }
  else
    return 0;
}

int is_FF_on(void){
  return flgs_ptr->FF_cntlr;
}

int is_FB_on(void){
  return flgs_ptr->FB_cntlr;
}

/* ----------------------------------------------------------------------------
 * Function init_adc()
 * ------------------------------------------------------------------------- */
int init_adc(void)
{
  unsigned int gpio;

  /* Set up ADC */
  if(adc_setup() != 1){
    printf("ADC setup failed\n");
    return -1;
  }

  /* Try each channel */
  if(get_adc_ain(ADC0, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC0);
    return -1;
  }
  s_ptr->adc_ch[0] = gpio;

  if(get_adc_ain(ADC1, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC1);
    return -1;
  }
  s_ptr->adc_ch[1] = gpio;

  if(get_adc_ain(ADC2, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC2);
    return -1;
  }
  s_ptr->adc_ch[2] = gpio;

  if(get_adc_ain(ADC3, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC3);
    return -1;
  }
  s_ptr->adc_ch[3] = gpio;

  if(get_adc_ain(ADC4, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC4);
    return -1;
  }
  s_ptr->adc_ch[4] = gpio;

  if(get_adc_ain(ADC5, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC5);
    return -1;
  }
  s_ptr->adc_ch[5] = gpio;

  if(get_adc_ain(ADC6, &gpio) != 1){
    printf("Error getting adc %s.\n",ADC6);
    return -1;
  }
  s_ptr->adc_ch[6] = gpio;
  printf("ADC initialized.\n");
  return 0;
}

int read_adc(void)
{
  for (int i=0; i<7; i++){
    if (read_value(s_ptr->adc_ch[i],&s_ptr->adc_value[i]) != 1){
      printf("ADC read failed, channel: %i.\n",s_ptr->adc_ch[i]);
      return -1;
    }
  }
  return 0;
}















