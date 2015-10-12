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
//#include "motor.h"
//#include "spi.h"
//#include "adc.h"
#include "control.h"

//#include "pruio.h"

/* Controller Structures */
anklestate_t *s_ptr;
cntlr_flgs_t *flgs_ptr;
config_t *config_ptr;
cleanup_t *cleanup_ptr;

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
  struct sigaction action_loop, action_cleanup, action_log;

  /* Allocate */
  s_ptr = malloc(sizeof(anklestate_t));
  flgs_ptr = malloc(sizeof(cntlr_flgs_t));
  config_ptr = malloc(sizeof(config_t));
  cleanup_ptr = malloc(sizeof(cleanup_t));

  /* Initialize */
  config_ptr->codeVersion = 0;
  config_ptr->frq_hz = frq_hz;
  config_ptr->dt_us = (suseconds_t)(1000000.0 / config_ptr->frq_hz);

  /* Initialize state */
  s_ptr->start_time.tv_sec = 0.0;
  s_ptr->start_time.tv_usec = 0.0;
  s_ptr->t_now.tv_sec = 0.0;
  s_ptr->t_now.tv_usec = 0.0;
  s_ptr->t_end.tv_sec = 0.0;
  s_ptr->t_end.tv_usec = 0.0;

  s_ptr->time_stamp = 0.0;
  s_ptr->cpu_time = 0.0;
  s_ptr->pos = 0.0;
  s_ptr->pos_0 = ANGLE_OFFSET;
  s_ptr->vel = 0.0;

  s_ptr->Kp = 100.0;
  s_ptr->Kd = 15.0;
  s_ptr->cmd = 0.0;

  s_ptr->FF_gain = 15.0;
  s_ptr->FF_period = 1.0;
  s_ptr->FF_phase = 0.0;

  for (int i=0; i<7; i++){
    s_ptr->adc_value[i] = 0.0;
  }

  flgs_ptr->control_loop_started = 0;
  flgs_ptr->FB_cntlr = 1;
  flgs_ptr->FF_cntlr = 0;

  cleanup_ptr->numOfFuncs = 0;

  if(create_log_file() != 0){
    printf("Cannot create log file.\n");
    control_cleanup(0);
    return -1;
  }

  printf("State Initialized.\n");

  /*
  if(init_motor() != 0){
    printf("Motor initialization error.\n");
    fprintf(config_ptr->f_log,"Motor initialization error.\n");
    control_cleanup(0);
    return -1;
  }
  */

  /*
  if(init_spi() != 0){
    printf("SPI initialization error.\n");
    control_cleanup(0);
    return -1;
  }
  */

  /*
  if(init_adc() != 0){
    printf("ADC initialization error.\n");
    control_cleanup(0);
    return -1;
  }
  */

  /* set handler for control loop signal */
  action_loop.sa_handler = control_loop_cb;
  sigemptyset(&action_loop.sa_mask);
  sigaddset(&action_loop.sa_mask, SIGIO);
  sigaddset(&action_loop.sa_mask, SIGUSR1);
  action_loop.sa_flags = 0;
  if(sigaction(SIGALRM, &action_loop, NULL) != 0){
    printf("Unable to catch SIGALRM\n");
    return -1;
  }

  /* set handler for log_data */
  action_log.sa_handler = log_data;
  sigemptyset(&action_log.sa_mask);
  action_log.sa_flags = 0;
  if(sigaction(SIGUSR1, &action_log, NULL) != 0){
    printf("Unable to catch SIGALRM\n");
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
 * Function add_func_to_cleanup() adds a cleanup function to be called on exit.
 * ------------------------------------------------------------------------- */
void add_func_to_cleanup(int (*FuncPtr)(void))
{
  cleanup_ptr->func[cleanup_ptr->numOfFuncs] = malloc(sizeof(FuncPtr));
  cleanup_ptr->func[cleanup_ptr->numOfFuncs] = FuncPtr;
  cleanup_ptr->numOfFuncs++;
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

  free(s_ptr);
  s_ptr = NULL;
  free(flgs_ptr);
  flgs_ptr = NULL;
  free(config_ptr);
  config_ptr = NULL;

  for(int i=0; i<cleanup_ptr->numOfFuncs; i++){
    cleanup_ptr->func[i]();
  }

  free(cleanup_ptr);
  cleanup_ptr = NULL;
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
  /* start time */
  if(gettimeofday(&s_ptr->t_now, NULL) == -1){
    printf("Error getting time.");
  }

  /* check control loop flag, set time stamp, and flag */
  if (flgs_ptr->control_loop_started != 0){
    set_time_stamp();
  }
  else{
    s_ptr->time_stamp = 0.0;
    set_start_time();
    flgs_ptr->control_loop_started = 1;
  }

  if (update_state() != 0){
    printf("State update failed\n");
  }

  /*
  if (update_control() != 0){
    printf("Control update failed\n");
  }
*/

  /* Get cpu time for control loop */
  if(gettimeofday(&s_ptr->t_end, NULL) == -1){
    printf("Error getting time.");
  }

  set_cpu_time();

  /* Raise signal to log data */
  raise(SIGUSR1);
}


/* ----------------------------------------------------------------------------
 * Function update_state() updates the controller states.
 *
 * TO DO: scale ADC appropriately
 * ------------------------------------------------------------------------- */
int update_state(void)
{
  /* Read current position of ankle encoder */
  /*
  if (read_pos() != 0){
    printf("State update failed -- read_pos().\n");
    return -1;
  }
  */

  /* Read all adc channels */
//  if (read_adc(s_ptr->adc_value) != 0){
//    printf("Error reading adc\n");
//    return -1;
//  }

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
  /*
  if(set_motor_duty(s_ptr->cmd) !=0){
    printf("pwm update failed.\n");
    return -1;
  }
*/
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function read_pos(*) is low level get ankle position using SPI.
 * ------------------------------------------------------------------------- */
int read_pos(void)
{

  /* send cmd */
/*  uint8_t msb[1] = {0x00};
  uint8_t lsb[1] = {0x00};
*/
  /* Send request 0x10 */
/*  uint8_t tx[1] = {0x10};
  uint8_t rx[ARRAY_SIZE(tx)] = {0x00,};
  if(spi_transfer(tx, rx) != 0){
    printf("spi transfer failed\n");
    return -1;
  }
*/
  /*
  printf("Send Request.\n");
  printf("tx: %x\n", tx[0]);
  printf("rx: %x\n", rx[0]);
  printf("\n");
  */

  /* send 0x00 till acknowledgement */
/*  tx[0] = 0x00;
  while(rx[0] != 0x10){
    if(spi_transfer(tx, rx) != 0){
      printf("spi transfer failed\n");
      return -1;
    }
*/
    /*
    printf("Wait for acknowledge.\n");
    printf("tx: %x\n", tx[0]);
    printf("rx: %x\n", rx[0]);
    printf("\n");
    */

  //}

  /* receive position */
/*  if(spi_transfer(tx, msb) != 0){
    printf("spi transfer failed\n");
    return -1;
  }
*/
  /*
  printf("MSB.\n");
  printf("tx: %x\n", tx[0]);
  printf("rx: %x\n", msb[0]);
  printf("\n");
  */
/*
  if(spi_transfer(tx, lsb) != 0){
    printf("spi transfer failed\n");
    return -1;
  }
*/
  /*
  printf("LSB.\n");
  printf("tx: %x\n", tx[0]);
  printf("rx: %x\n", lsb[0]);
  printf("\n");
  */

 // s_ptr->pos = encoder_to_angle(msb[0],lsb[0]);
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function encoder_to_angle(*,*) converts encoder counts to radians.
 * ------------------------------------------------------------------------- */
float encoder_to_angle(uint8_t msb, uint8_t lsb)
{
  float angle = ((float)((msb << 8) | (lsb & 0xff)))*2.0*M_PI/4096.0;
  if(angle > M_PI){
    angle = angle - 2.0*M_PI;
  }
  return angle + ANGLE_OFFSET;
}

/* ----------------------------------------------------------------------------
 * Log data
 * ------------------------------------------------------------------------- */
void log_data(int signum)
{
 fprintf(config_ptr->f_log,
          "%e "
          "%e\n",
          s_ptr->time_stamp,
          s_ptr->cpu_time);

  if(debug){
    printf("%e "
          "%e\n",
          s_ptr->time_stamp,
          s_ptr->cpu_time);
  }
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

void set_start_time(void)
{
  s_ptr->start_time = s_ptr->t_now;
}

/* ----------------------------------------------------------------------------
 * Function get_time_stamp(*) creates time stamp for data.
 * ------------------------------------------------------------------------- */
void set_time_stamp(void)
{
  s_ptr->time_stamp = (float)(s_ptr->t_now.tv_sec - s_ptr->start_time.tv_sec)
      + (float)(s_ptr->t_now.tv_usec - s_ptr->start_time.tv_usec)/1.0e6;
}

/* ----------------------------------------------------------------------------
 * Function get_time_interval(*,*) calculates interval.
 * ------------------------------------------------------------------------- */
void set_cpu_time(void)
{
  s_ptr->cpu_time = (float)(s_ptr->t_end.tv_sec - s_ptr->t_now.tv_sec)
      + (float)(s_ptr->t_end.tv_usec - s_ptr->t_now.tv_usec)/1.0e6;
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

















