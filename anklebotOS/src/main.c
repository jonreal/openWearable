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
#include "tui.h"

/* Matlab Helper */
#include "/Applications/MATLAB_R2013a.app/extern/include/mat.h"

volatile int doneFlag = 0;
float freq_hz = 1000.0;
int debug;

/* Default Parameters */
param_mem_t defaults = {1000 /* Freq. (Hz) */,
                        0 /* Freq. Clock ticks (set as zero!) */,
                        0 /* Subject Mass */,
                        0 /* toe_hs threshold */,
                        0 /* mid_hs threshold */,
                        0 /* heel_hs threshold */,
                        0 /* toe_to threshold */,
                        0 /* mid_to threshold */,
                        0 /* heel_to threshold */,
                        0 /* gpOnLeftFoot */,
                        0 /* Kp */,
                        0 /* Kd */,
                        0 /* anklePos0 */};

/* Default FF lookup table */
ff_mem_t ff_defaults = {{0},};



void sigintHandler(int sig)
{
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

int main(int argc, char **argv)
{
  int rtn = 0;

  int i;
  for(i=0; i<1000; i++){
    printf("%i\n", ff_defaults.ff_traj[i]);
  }
  return;

  /* Command Line Inputs */
  if(argc != 1){
    if(strcmp(argv[1], "-v") == 0){
      debug = 1;
    }
  }
  else{
    debug = 0;
  }


  if(!(debug)){
    printf("\n---------------------\n");
    printf("Welcome to AnklebotOS\n");
    printf("---------------------\n");
  }
  else{
    printf("\n---------------------\n");
    printf(" DEBUG MODE \n");
    printf("---------------------\n");
    signal(SIGINT, sigintHandler);
  }

  printf("Control loop: \n\t%.2f (Hz), (0x%x)\n", freq_hz,  hzToPruTicks(freq_hz));

  /* initialize the library, PRU and interrupt */
  if((rtn = pru_init()) != 0){
    return -1;
  }

  /* Init debug buffer */
  initDebugBuffer();

  /* Init params */
  writeParams(defaults);

  /* Feedforward lookup */
//  writePruConrtolParams(1, 0, 0, feedForward);

 /* Run PRU0 software */
  if( (rtn = pru_run(PRU_SENSOR, "./bin/pru0_main_text.bin")) != 0){
    printf("pru_run() failed (PRU_SENSOR)");
    return -1;
  }

  /* Run PRU1 software */
  if( (rtn = pru_run(PRU_CONTROL, "./bin/pru1_main_text.bin")) != 0){
    printf("pru_run() failed (PRU_CONTROL)");
    return -1;
  }

  clearFlowBitFeild();

  printf("Press enter to start");
  getchar();

  /* Enable pru sensor/control */
  enable();

  /* Start control loop */
  armToPru1Interrupt();
  armToPru0Interrupt();


  if(debug){

    uint8_t lastBufferRead = 0;
    while(!(doneFlag)){

      if(isBuffer0Full()){
        resetBuffer0FullFlag();
        //  gpio_set_value(gpio_debug, HIGH);
        writeState(0);
        lastBufferRead = 0;
        //  gpio_set_value(gpio_debug, LOW);
      }

      if(isBuffer1Full()){
        resetBuffer1FullFlag();
        //  gpio_set_value(gpio_debug, HIGH);
        writeState(1);
        lastBufferRead = 1;
        //   gpio_set_value(gpio_debug, LOW);
      }
    }
    disable();
    if(lastBufferRead == 1){
      writeState(0);
    }
    else{
      writeState(1);
    }
    printDebugBuffer();

    /* Cleanup */
    pru_cleanup();
    printf("pru0 and pru1 cleaned up.\n");
  }
  else {

    if(init_tui() != 0){
      printf("TUI init fail.");
    }


    /* UI loop */
    if(start_tui() == 1){
        disable();
        printDebugBuffer();

        /* Cleanup */
        pru_cleanup();
        printf("pru0 and pru1 cleaned up.\n");
        raise(SIGINT);
    }
  }
  return 0;
}


