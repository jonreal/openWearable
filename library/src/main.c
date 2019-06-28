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
#include "debug.h"
#include "pru.h"
#include "log.h"
#include "tui.h"
#include "format.h"


typedef enum {
  DEBUG = 0,
  TUI,
} ow_mode_t;

volatile int doneFlag = 0;

void sigintHandler(int sig) {
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

// ---------------------------------------------------------------------------
int main(int argc, char **argv) {

  char buff[65536] = {0,};
  ow_mode_t mode;
  if (argc != 1) {
    if (strcmp(argv[1], "-v") == 0) {
      mode = DEBUG;
    } else {
      mode = TUI;
    }
  } else {
    mode = TUI;
  }

  if (DebugInit() != 0) {
    printf("DebugInit() failed.");
    return -1;
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
  FormatSprintParams(pru_mem.p, buff);
  fprintf(stdout,buff);

  if (PruLoadLut("config/sine_1Hz",pru_mem.l) != 0) {
    printf("\nLookup table file not found!\n");
  }

  if(PruInit(FWSUFFIX) != 0)
    return -1;

  switch (mode) {

    case DEBUG:
      printf("\n"
             "-----------------------------\n"
             " Welcome to openWearable v0.1\n"
             "--------- DEBUG MODE --------\n"
             "-----------------------------\n");

      printf("\n\nPress enter to start\n\n");
      getchar();
      PruEnable(1, &pru_mem.s->pru_ctl);

      signal(SIGINT, sigintHandler);
      circbuff_t* cb = LogNewCircBuff();
      while (!doneFlag) {
        LogDebugWriteState(pru_mem.s, cb, buff);
      }
      PruEnable(0, &pru_mem.s->pru_ctl);
      free(cb);
      PruPrintDebugBuffer(pru_mem.p->debug_buff);
      break;

    case TUI:
      printf("\n"
             "-----------------------------\n"
             " Welcome to openWearable v0.1\n"
             "-----------------------------\n");

      printf("\n\nPress enter to start\n\n");
      getchar();
      PruEnable(1, &pru_mem.s->pru_ctl);

      if (TuiInit() != 0) {
        printf("Tui init failed.\n");
        return -1;
      }
      if (TuiLoop(&pru_mem) == 1) {
        PruEnable(0, &pru_mem.s->pru_ctl);
        PruRestart();
        PruPrintDebugBuffer(pru_mem.p->debug_buff);
        TuiCleanup();
        DebugCleanup();
        raise(SIGINT);
      }
      break;
  }
  return 0;
}
