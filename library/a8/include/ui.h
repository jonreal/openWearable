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

#ifndef _TUI_H__
#define _TUI_H_

#include "pru.h"
#include <pthread.h>
#include "log.h"
#include "udp.h"

typedef struct {
  int debug;
  int logging;
  int logfile;
//  int rospublish;
  int udppublish;
  char udphost[64];
  int nodma;        // Flag to disable DMA transfers
  int show_stats;   // Flag to show performance statistics
} ui_flags_t;

typedef struct {
  unsigned int counter;
  ui_flags_t flag;
  pru_mem_t* pru_mem;
  cpudata_t* cpudata;
  log_t* log;
  udp_t* udp;
//  rospub_t* ros;
//  char rosbuffer[1024];
} ui_data_t;

extern int UiLoop(const pru_mem_t* pru_mem);
extern void UiPrintMenu(const pru_mem_t* pru_mem);

ui_flags_t UiInitFlags(void);
int UiInit(pru_mem_t* pru_mem, ui_flags_t flags);
int UiCleanup(void);
void UiNewLogFile(char* log_file);
void UiStartLog(void);
void UiStopAndSaveLog(void);

void UiWelcome(void);
void UiPollForUserInput(void);
void UiSetCmd(const pru_mem_t* pru_mem, uint32_t mask);    // A8->PRU: assert command
void UiClearCmd(const pru_mem_t* pru_mem, uint32_t mask);  // A8->PRU: deassert command
int  UiGetCmd(const pru_mem_t* pru_mem, uint32_t mask);    // read own command bit
int  UiSignaled(const pru_mem_t* pru_mem, uint32_t mask);  // PRU->A8: any core signalled?
void UiPollSignal(const pru_mem_t* pru_mem, uint32_t mask, int value); // block until signal==value

int UiLogging(void);

#endif /* _TUI_H_ */
