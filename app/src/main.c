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
#include <sys/mman.h>
#include <fcntl.h>

#include "mem_types.h"
#include "pru.h"
#include "log.h"
#include "tui.h"

int debug;
volatile int doneFlag = 0;

void sigintHandler(int sig) {
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

// ---------------------------------------------------------------------------
int main(int argc, char **argv) {
  char buff[65536] = {0,};

  if (argc != 1) {
    if (strcmp(argv[1], "-v") == 0) debug = 1;
  } else {
    debug = 0;
  }

  if (!debug) {
    printf("\n---------------------\n");
    printf("Welcome to openWearable v0.1\n");
    printf("---------------------\n");
  } else {
    printf("\n---------------------\n");
    printf(" DEBUG MODE \n");
    printf("---------------------\n");
    signal(SIGINT, sigintHandler);
  }

  pru_mem_t pru_mem;
  if (PruMemMap(&pru_mem) != 0) {
    printf("pru_mem_init() failed.");
    return -1;
  }

  if (PruLoadParams("config/default", pru_mem.p) != 0) {
    printf("\nParameter file not found!\n");
  }
  buff[0] = '\0';
  PruSprintParams(pru_mem.p, buff);
  fprintf(stdout,buff);

  if (PruLoadIirFilterParams("config/lowpass_1_6Hz", &pru_mem.p->iir_param)) {
      printf("\nFilter coefficient file not found!\n");
  }
  buff[0] = '\0';
  PruSprintIirParams(&pru_mem.p->iir_param, buff);
  fprintf(stdout,buff);

  if (PruLoadNlbFilterParams("config/nonlinBayes", &pru_mem.p->nlb_param)) {
      printf("\nBayes filter parameters file not found!\n");
  }
  buff[0] = '\0';
  PruSprintNlbParams(&pru_mem.p->nlb_param, buff);
  fprintf(stdout,buff);

  if(PruInit() != 0)
    return -1;

  // Start control loop
  printf("\n\nPress enter to start\n\n");
  getchar();

  PruEnable(1, &pru_mem.s->pru_ctl);

  if(debug){
    circbuff_t* cb = LogNewCircBuff();
    while(!(doneFlag)){
      LogDebugWriteState(pru_mem.s, cb, buff);
    }
    PruEnable(0, &pru_mem.s->pru_ctl);
    free(cb);
    PruPrintDebugBuffer(pru_mem.p->debug_buff);
  } else {
    if (TuiInit() != 0) {
      printf("TUI init fail.");
      return -1;
    }
    if(TuiLoop(&pru_mem) == 1){
      PruEnable(0, &pru_mem.s->pru_ctl);
      PruPrintDebugBuffer(pru_mem.p->debug_buff);
      raise(SIGINT);
    }
  }
  return 0;
}

/* Methods ----------------------------------------------------------------- */

