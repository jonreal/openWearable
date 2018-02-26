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

volatile sig_atomic_t input_ready;

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

void TuiInputCallback(int sig) {
  input_ready = 1;
}

void TuiPrintMenu(void) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: f - Collect trial\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
}

int TuiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

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
        case 'e' :
          return 1;

        // ---- Collect trial -------------------------------------------------
        case 'f' :
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

          log_t* log = LogFileOpen(pru_mem, log_file);

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
          while (1) {
            LogWriteStateToFile(pru_mem, log);
            if(input_ready)
              break;
          }
          scanf(" %c", &input_char);
          LogFileClose(log);
          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          fflush(stdout);
          input_ready = 0;
          break;
      }
    }
  }
}

int TuiCleanup(void)
{
  if(fcntl(0, F_SETOWN, NULL) == -1){
    printf("F_SETOWN error.\n");
    return -1;
  }
  if(fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_ASYNC & ~O_NONBLOCK) == -1){
    printf("Error setting stdin fd flags.\n");
    return -1;
  }

  printf("TUI cleaned up.\n");
  return 0;
}
