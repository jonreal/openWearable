/* Copyright 2017 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "mem_types.h"
#include "pru_wrappers.h"
#include "gpio.h"
#include "common.h"
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
    printf("Welcome to openWearable v0.1\n");
    printf("---------------------\n");
  }
  else{
    printf("\n---------------------\n");
    printf(" DEBUG MODE \n");
    printf("---------------------\n");
    signal(SIGINT, sigintHandler);
  }

  // Map pru mem to userspace
  if(pru_mmap() != 0){
    printf("pru_mem_init() failed.");
    return -1;
  }

  // Load parameters from file to memory.
  if(loadParameters("config/default") != 0){
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

  // Initialize the library, PRUs and interrupts.
  if(pru_init() != 0)
    return -1;

  // Start control loop
  printf("\n\nPress enter to start\n\n");
  getchar();

  // Start PRUs
  enablePru(1);

  // Debug Loop
  if(debug){
    circBuffInit();
    while(!(doneFlag)){
      logData();
    }
    enablePru(0);
    printDebugBuffer();
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

