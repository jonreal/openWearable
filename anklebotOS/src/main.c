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

/* Global ------------------------------------------------------------------ */
int debug;
volatile int doneFlag = 0;

/* Protoypes --------------------------------------------------------------- */
void sigintHandler(int sig);
int loadPruSoftware(void);
void startPruLoop(void);

/* ------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  uint8_t lastBufferRead = 0;

  /* Command Line Inputs */
  if(argc != 1){
    if(strcmp(argv[1], "-v") == 0)
      debug = 1;
  }
  else
    debug = 0;

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

  /* initialize the library, PRU and interrupt */
  if(pru_init() != 0)
    return -1;

  /* Init debug buffer */
  initDebugBuffer();

  /* Load default params */
  loadParameters("config/defaults");
  printParameters();

  /* Feedforward lookup */
  loadLookUpTable("config/constant");
  //printFFLookUpTable();

  /* Load Pru */
  if(loadPruSoftware() != 0)
    return -1;

  clearFlowBitFeild();

  printf("Press enter to start");
  getchar();

  /* Enable pru sensor/control */
  enable();

  /* Start control loop */
  startPruLoop();

  /* Debug Loop */
  if(debug){
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

  /* TUI */
  else {

    if(init_tui() != 0){
      printf("TUI init fail.");
      return -1;
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

/* Methods ----------------------------------------------------------------- */
void sigintHandler(int sig)
{
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

int loadPruSoftware(void)
{
  int rtn = 0;

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
  return rtn;
}

void startPruLoop(void)
{
  armToPru1Interrupt();
  armToPru0Interrupt();
}
