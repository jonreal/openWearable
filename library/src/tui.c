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
#include "pruloop.h"

typedef struct {
  int logflag;
  log_t* log;
  udp_t* udp;
} tui_thread_data_t;

volatile sig_atomic_t input_ready;
pthread_mutex_t lock;
tui_thread_data_t thread_data;


// Static Functions
static void* TuiLogAndPublishThread(void* args);
static void TuiInputCallback(int sig);
static void TuiPrintMenu(void);


// ---------------------------------------------------------------------------
static void TuiInputCallback(int sig) {
  input_ready = 1;
}

static void TuiPrintMenu(void) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: f - Collect trial\n"
  "      s - Start Pronation/Supination Exp\n"
  "      d - Start Tracking Exp\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
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
                                  thread_data.udp);
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



int TuiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  float input_float = 0.0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  // initialize thread data
  thread_data.logflag = 0;
  thread_data.udp = UdpInit();
  thread_data.log = LogInit(pru_mem);

  pthread_t thread;
  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\n mutex init failed\n");
    return 1;
  }
  if (pthread_create(&thread, NULL, &TuiLogAndPublishThread, NULL)) {
    printf("\n thread create failed\n");
    return 1;
  }

  TuiPrintMenu();
  while (1) {
    // Clear inputs
    input_char = ' ';
    input_string[0] = '\0';

    // Wait for user input.
    if (input_ready) {
      scanf(" %c", &input_char);

      switch(input_char){

        // ---- Exit ----------------------------------------------------------
        case 'e' : {
          pthread_mutex_lock(&lock);
          thread_data.logflag = -1;
          pthread_mutex_unlock(&lock);
          pthread_join(thread, NULL);
          printf("done.\n");
          return 1;
        }

        // ---- Collect trial -------------------------------------------------
        case 'f' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          input_ready = 0;

          // Wait for input.
          while (1)
            if (input_ready)
              break;

          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);

          pthread_mutex_lock(&lock);
          LogNewFile(thread_data.log, log_file);
          pthread_mutex_unlock(&lock);

          // Wait for enter to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          input_ready = 0;
          while (1)
            if (input_ready)
              break;
          scanf(" %c", &input_char);

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);
          input_ready = 0;

          // Data collection loop
          pthread_mutex_lock(&lock);
          thread_data.logflag = 1;
          pthread_mutex_unlock(&lock);
          while (1) {
            if(input_ready)
              break;
          }
          pthread_mutex_lock(&lock);
          thread_data.logflag = 0;
          LogSaveFile(thread_data.log);
          pthread_mutex_unlock(&lock);
          scanf(" %c", &input_char);

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          fflush(stdout);
          input_ready = 0;
          break;
        }

        // ---- Start exp -------------------------------------------------
        case 's' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          input_ready = 0;

          // Wait for input.
          while (1)
            if (input_ready)
              break;

          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);

          pthread_mutex_lock(&lock);
          LogNewFile(thread_data.log, log_file);
          pthread_mutex_unlock(&lock);


          // Wait for enter to start saving data
          printf("\t\tEnter desired pressure (psi): ");
          fflush(stdout);
          input_ready = 0;
          while (1)
            if (input_ready)
              break;
          scanf(" %f", &input_float);
          pru_mem->p->p_d_static = fix16_from_float(input_float);

          // Wait for enter to start saving data
          printf("\t\tEnter desired angle (deg): ");
          fflush(stdout);
          input_ready = 0;
          while (1)
            if (input_ready)
              break;
          scanf(" %f", &input_float);
          pru_mem->p->theta_d = fix16_from_float(input_float);


          printf("\t\tp_d = %f, theta_d=%f\n",
                fix16_to_float(pru_mem->p->p_d_static),
                fix16_to_float(pru_mem->p->theta_d));


          // Wait for enter to start saving data
          printf("\t\tPress enter to start experiment...");
          fflush(stdout);
          input_ready = 0;
          while (1)
            if (input_ready)
              break;
          scanf(" %c", &input_char);
          printf("\t\t\trunning experiment...\n");
          fflush(stdout);
          pru_mem->s->pru_ctl.bit.utility |= (1 << 0);

          // Data collection loop
          pthread_mutex_lock(&lock);
          thread_data.logflag = 1;
          pthread_mutex_unlock(&lock);
          while (1) {
            if (~(pru_mem->s->pru_ctl.bit.utility & (1 << 0)))
              break;
          }
          pthread_mutex_lock(&lock);
          thread_data.logflag = 0;
          LogSaveFile(thread_data.log);
          pthread_mutex_unlock(&lock);

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          fflush(stdout);
          input_ready = 0;
          pru_mem->p->theta_d = 0;
          pru_mem->p->p_d_static = 0;
          break;
        }

        // ---- Start exp -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          input_ready = 0;

          // Wait for input.
          while (1)
            if (input_ready)
              break;

          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);

          pthread_mutex_lock(&lock);
          LogNewFile(thread_data.log, log_file);
          pthread_mutex_unlock(&lock);

          // Wait for enter to start saving data
          printf("\t\tPress enter to start experiment...");
          fflush(stdout);
          input_ready = 0;
          while (1)
            if (input_ready)
              break;
          scanf(" %c", &input_char);
          printf("\t\t\trunning experiment...\n");
          fflush(stdout);
          pru_mem->s->pru_ctl.bit.utility |= (1 << 0);

          // Data collection loop
          pthread_mutex_lock(&lock);
          thread_data.logflag = 1;
          pthread_mutex_unlock(&lock);
          while (1) {
            if (~(pru_mem->s->pru_ctl.bit.utility & (1 << 0)))
              break;
          }
          pthread_mutex_lock(&lock);
          thread_data.logflag = 0;
          LogSaveFile(thread_data.log);
          pthread_mutex_unlock(&lock);


          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          fflush(stdout);
          input_ready = 0;
          pru_mem->p->theta_d = 0;
          pru_mem->p->p_d_static = 0;
          break;
        }
      }
    }
  }
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
  printf("TUI cleaned up.\n");
  return 0;
}


