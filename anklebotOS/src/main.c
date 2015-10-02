#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "mem_types.h"
#include "pru_wrappers.h"
#include "gpio.h"
#include "common.h"
#include "control.h"

#define DEBUG_PIN "P8_15"

volatile int doneFlag = 0;
float freq_hz = 100.0;
int debug;
FILE* fid;

void sigintHandler(int sig)
{
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

int logFileInit(void)
{
  char logstr[256] = "datalog/log_BB_";
  char timestr[256];

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  /* Create log file */
  strftime(timestr, sizeof(timestr)-1, "%Y_%m_%d_%H-%M", t);
  strcat(logstr, timestr);
  strcat(logstr, ".txt");
  fid = fopen(logstr, "w");
  printf("Log file: \n\t%s\n",logstr);

  /* Create header */
  fprintf(fid,"%% anklebot \n");
  fprintf(fid,"%% Date: %s\n", timestr);
  fprintf(fid,"%% Sample frequency = %f\n", freq_hz);

  return 0;
}

int main(int argc, char **argv)
{
  int rtn = 0;
  uint8_t buffIndx = 0;
  uint32_t gpio_debug;

  /* Inputs */
  uint32_t feedForward[100] = {0,};

  signal(SIGINT, sigintHandler);


  printf("\n---------------------\n");
  printf("Welcome to AnklebotOS\n");
  printf("---------------------\n");

  logFileInit();

  printf("Control loop: \n\t%.2f (Hz), (0x%x)\n", freq_hz,  hzToPruTicks(freq_hz));


  /* Debug pin */
  /* set enable pins */
  if(get_gpio_number(DEBUG_PIN, &gpio_debug) != 0){
    printf("Error getting gpio number.\n");
    return -1;
  }
  gpio_export(gpio_debug);
  gpio_set_direction(gpio_debug, OUTPUT);

  /* initialize the library, PRU and interrupt */
  if((rtn = pru_init()) != 0){
    return -1;
  }

  /* Init sensor params */
  writePruSensorParams(freq_hz, 0xDEADBEAF, 0xBEAFDEAD, 0xFFFFFFFF);

  /* Init control params */
  writePruConrtolParams(10, 10, 10, feedForward);

 /* Run PRU0 software */
  if( (rtn = pru_run(PRU_SENSOR, "./bin/pru0_sensor_text.bin")) != 0){
    printf("pru_run() failed (PRU_SENSOR)");
    return -1;
  }

  /* Run PRU1 software */
  if( (rtn = pru_run(PRU_CONTROL, "./bin/pru1_control_text.bin")) != 0){
    printf("pru_run() failed (PRU_CONTROL)");
    return -1;
  }

  clearFlowBitFeild();

  printf("\nPress enter to begin data collection:\n ");
  getchar();

  /* Enable pru sensor/control */
  enable();

  /* Start control loop */
  armToPru1Interrupt();
  armToPru0Interrupt();


  while(!(doneFlag)){
    if(isBufferFull()){
      resetBufferFullFlag();

      gpio_set_value(gpio_debug, HIGH);

      writeState(buffIndx);

      buffIndx++;
      if(buffIndx == NUM_OF_BUFFS)
        buffIndx = 0;

      gpio_set_value(gpio_debug, LOW);
    }
  }

  disable();
  writeState(buffIndx);

  if(buffIndx == 1){
    writeState(0);
  }
  else{
    writeState(1);
  }

  fclose(fid);

  /* Cleanup */
  pru_cleanup();
  printf("pru0 and pru1 cleaned up.\n");

  return 0;
}


