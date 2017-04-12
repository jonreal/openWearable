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

// Global --------------------------------------------------------------------
int debug;
volatile int doneFlag = 0;

// Protoypes -----------------------------------------------------------------
void sigintHandler(int sig);
int loadPruSoftware(void);
void startPruLoop(void);

// ---------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // Command Line Inputs
  if(argc != 1){
    if(strcmp(argv[1], "-v") == 0)
      debug = 1;
  }
  else{
    debug = 0;
  }

  // Welcome
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

  // Initialize the library, PRUs and interrupts.
  if(pru_init() != 0)
    return -1;

  // Initialize debug buffer.
  initDebugBuffer();

  // Load parameters from file to memory.
  if(loadParameters("config/PA_A02") != 0){
    printf("\nParameter file not found!\n");
  }
  printParameters(stdout);

  // Load iir filter coefficients from file to memory.
  if(loadIirFilterCoeff("config/lowpass_1_6Hz")){
      printf("\nFilter coefficient file not found!\n");
  }
  printFirCoeff(stdout);

  // Load lookup table from file to memory.
  if(loadLookUpTable("config/sine_13amps_1Hz") != 0){
    printf("\nLookup table file not found!\n");
  }
//  printFFLookUpTable(stdout);

  // Load binaries to PRUs IRAM.
  if(loadPruSoftware() != 0)
    return -1;

  clearFlowBitField();

  // Enable PRUs
  enablePru(1);

  // Start control loop
  printf("\n\nPress enter to start\n\n");
  getchar();

  startPruLoop();

  // Debug Loop
  if(debug){
    circBuffInit();
    while(!(doneFlag)){
      logData();
    }
    enablePru(0);
    printDebugBuffer();

    // Cleanup
    pru_cleanup();
    printf("pru0 and pru1 cleaned up.\n");
  }

  // TUI
  else {
    if(init_tui() != 0){
      printf("TUI init fail.");
      return -1;
    }

    // UI loop
    if(start_tui() == 1){
        enablePru(0);
        printDebugBuffer();

        // Cleanup
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

  // Run PRU0 software
  if( (rtn = pru_run(PRU0, "./bin/pru0_main_text.bin")) != 0){
    printf("pru_run() failed (PRU0)");
    return -1;
  }

  // Run PRU1 software
  if( (rtn = pru_run(PRU1, "./bin/pru1_main_text.bin")) != 0){
    printf("pru_run() failed (PRU1)");
    return -1;
  }
  return rtn;
}

void startPruLoop(void)
{
  armToPru1Interrupt();
  armToPru0Interrupt();
}
