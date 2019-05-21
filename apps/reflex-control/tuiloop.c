#include "tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void TuiPrintMenu(void) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: f - Collect trial\n"
  "      s - Start Pronation/Supination Exp\n"
  "      d - Start Tracking Exp\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
}

int TuiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  TuiInitLogAndPublishThread(pru_mem);
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
          TuiCloseLogAndPublishThread();
          printf("done.\n");
          return 1;
        }

        // ---- Collect trial -------------------------------------------------
        case 'f' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          TuiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          TuiPollForUserInput();
          scanf(" %c", &input_char);

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);

          // Data collection loop
          TuiStartLog();
          TuiPollForUserInput();
          TuiStopAndSaveLog();
          scanf(" %c", &input_char);

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          break;
        }

        // ---- Start exp -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          input_ready = 0;

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          TuiNewLogFile(log_file);

          // Wait for enter to start saving data
          printf("\t\tPress enter to start experiment...");
          fflush(stdout);
          TuiPollForUserInput();
          scanf(" %c", &input_char);
          printf("\t\t\trunning experiment...\n");
          fflush(stdout);
          TuiSetPruCtlBit(pru_mem, 0);

          // Data collection loop
          TuiStartLog();
          TuiPollPruCtlBit(pru_mem, 0);
          TuiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu();
          pru_mem->p->theta_d = 0;
          pru_mem->p->p_d_static = 0;
          break;
        }
      }
    }
  }
}

