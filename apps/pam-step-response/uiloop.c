#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " holdtime = %f,\t which-pam = %i\n\n"
  "Menu: s - start step\n"
  "      d - change holdtime\n"
  "      f - change whichpam\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  ((float)pru_mem->p->holdtime)/1000.0, pru_mem->p->whichpam);
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  float input_float = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  UiPrintMenu(pru_mem);
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
          UiStopAndSaveLog();
          printf("done.\n");
          return 1;
        }

        // ---- Start data collection -----------------------------------------
        case 's' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          // Data collection loop
          printf("\t\tStep Response ongoing...\n");
          UiStartLog();
          UiSetPruCtlBit(pru_mem, 0);

          UiPollPruCtlBit(pru_mem, 0, 0);
          UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change time -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter new holdtime (s): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->holdtime = (uint32_t)(input_float * 1000.0);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change time -------------------------------------------------
        case 'f' : {
          printf("\t\tEnter new whichpam (0 | 1 | 2): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->whichpam = (uint32_t)(input_float);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

