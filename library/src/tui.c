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

#include "tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "log.h"

#include "../../ros/roshelper.h"

typedef struct {
  int logflag;
  log_t* log;
  udp_t* udp;
  rospub_t* rp;
} tui_thread_data_t;

// Global
volatile sig_atomic_t input_ready;
pthread_mutex_t lock;
tui_thread_data_t thread_data;
pthread_t thread;

// ---------------------------------------------------------------------------
static void TuiInputCallback(int sig) {
  input_ready = 1;
}

static void* TuiLogAndPublishThread(void* args) {
  while (1) {
    pthread_mutex_lock(&lock);

    if (thread_data.logflag == -1) {
      printf("\nThread exiting...");
      return NULL;
    }

    LogWriteStateToFileAndPublish(thread_data.logflag,
                                  thread_data.log,
                                  thread_data.udp,
                                  thread_data.rp);
    pthread_mutex_unlock(&lock);
  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Function init_tui()
// ----------------------------------------------------------------------------
int TuiInit(void) {

  // Setup action for SIGIO for user inputs
  struct sigaction action_ui;
  action_ui.sa_handler = TuiInputCallback;
  sigemptyset(&action_ui.sa_mask);
  action_ui.sa_flags = 0;
  if (sigaction(SIGIO, &action_ui, NULL) == -1)
    printf("Error sigaction\n");

  // Setup stdin flags
  if (fcntl(0, F_SETOWN, getpid()) == -1) {
    printf("F_SETOWN error.\n");
    return -1;
  }
  if (fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_ASYNC | O_NONBLOCK) == -1) {
    printf("Error setting stdin fd flags.\n");
    return -1;
  }
  printf("TUI initialized.\n");
  fflush(stdout);
  return 0;
}

int TuiInitLogAndPublishThread(const pru_mem_t* pru_mem) {
  thread_data.logflag = 0;
  thread_data.udp = UdpInit();
  thread_data.log = LogInit(pru_mem);
  thread_data.rp = RosPubInit();

  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\n mutex init failed\n");
    return 1;
  }
  if (pthread_create(&thread, NULL, &TuiLogAndPublishThread, NULL)) {
    printf("\n thread create failed\n");
    return 1;
  }

  return 0;
}

void TuiCloseLogAndPublishThread(void) {
  pthread_mutex_lock(&lock);
  thread_data.logflag = -1;
  pthread_mutex_unlock(&lock);
  pthread_join(thread, NULL);
}

void TuiStartLog(void) {
  pthread_mutex_lock(&lock);
  thread_data.logflag = 1;
  pthread_mutex_unlock(&lock);
}

void TuiStopAndSaveLog(void) {
  pthread_mutex_lock(&lock);
  thread_data.logflag = 0;
  LogSaveFile(thread_data.log);
  pthread_mutex_unlock(&lock);
}

void TuiNewLogFile(char* log_file) {
  pthread_mutex_lock(&lock);
  LogNewFile(thread_data.log, log_file);
  pthread_mutex_unlock(&lock);
}


void TuiPollForUserInput(void) {
  input_ready = 0;
  while (1)
    if (input_ready)
      break;
}

void TuiSetPruCtlBit(const pru_mem_t* pru_mem, unsigned char n) {
  pru_mem->s->pru_ctl.bit.utility |= (1 << n);
}

void TuiPollPruCtlBit(const pru_mem_t* pru_mem, unsigned char n) {
  while (1)
    if ((pru_mem->s->pru_ctl.bit.utility & (1 << n)) == 0)
      break;
}

int TuiCleanup(void) {
  if(fcntl(0, F_SETOWN, NULL) == -1){
    printf("F_SETOWN error.\n");
    return -1;
  }
  if(fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_ASYNC & ~O_NONBLOCK) == -1){
    printf("Error setting stdin fd flags.\n");
    return -1;
  }
  LogCleanup(thread_data.log);
  RosPubCleanup(thread_data.rp);
  printf("TUI cleaned up.\n");
  return 0;
}

