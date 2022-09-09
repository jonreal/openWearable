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
#include "ui.h"
#include "format.h"

volatile sig_atomic_t doneFlag;

static void sigintHandler(int sig) {
  doneFlag = 1;
  signal(sig, SIG_IGN);
}

// ---------------------------------------------------------------------------
int main(int argc, char **argv) {
  doneFlag = 0;
  char buff[65536] = {0,};
  pru_mem_t pru_mem;
  int c;
  ui_flags_t uiflags = UiInitFlags();
  char configfile[256] = "/root/openWearable/apps/config/";
  int configFlag = 0;
  while((c = getopt(argc, argv, "vurc:")) != -1) {
    switch (c) {
      case 'v':
        uiflags.debug = 1;
        break;
      //case 'r':
      //  uiflags.rospublish = 1;
      //  break;
      case 'u':
        uiflags.udppublish = 1;
        break;
      case 'c':
        strcat(configfile,optarg);
        printf("\nloading config file %s\n\n", configfile);
        configFlag = 1;
        break;
    }
  }

  if (PruMemMap(&pru_mem) != 0) {
    printf("pru_mem_init() failed.");
    return -1;
  }
  if (configFlag) {
    if (PruLoadParams(configfile, pru_mem.p) != 0) {
      printf("\nParameter file not found!\n");
      return -1;
    }
  }
  else {
    if (PruLoadParams(NULL, pru_mem.p) != 0) {
      return -1;
    }
  }
  buff[0] = '\0';
  FormatSprintParams(pru_mem.p, buff);
  fprintf(stdout,buff);
  if (PruLoadLut("/root/openWearable/apps/config/sine_1Hz",pru_mem.l) != 0) {
    printf("\nLookup table file not found!\n");
  }
  if(PruInit(FWSUFFIX) != 0)
    return -1;

  UiWelcome();
  if (UiInit(&pru_mem,uiflags) != 0) {
    printf("Tui init failed.\n");
    return -1;
  }

  if (uiflags.debug) {
    PruEnable(1, &pru_mem.s->pru_ctl);
    signal(SIGINT, sigintHandler);
    circbuff_t* cb = LogNewCircBuff();
    while (!doneFlag) {
      LogDebugWriteState(pru_mem.s, cb, buff);
    }
    PruPrintDebugBuffer(pru_mem.p->debug_buff);
    PruEnable(0, &pru_mem.s->pru_ctl);
    PruRestart();
  } else {
    PruEnable(1, &pru_mem.s->pru_ctl);
    if ((UiLoop(&pru_mem) == 1) | (doneFlag)){
      PruEnable(0, &pru_mem.s->pru_ctl);
      PruRestart();
    }
  }
  UiCleanup();
  return 0;
}
