#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

static void UiParamMenu(void) {
  printf(
    "\n"
    "\t\t a - P0 - base pressure\n"
    "\t\t s - dP - reflex step size\n"
    "\t\t d - thrs - reflex threshold\n"
    );
}

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \tP0 = %3.2f,\tdP = %3.2f,\tthrs = %3.2f\n\n"
  "Menu: f - start trial\n"
  "      s - stop trial\n"
  "      p - change parameter\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->p_0),
  fix16_to_float(pru_mem->p->dp),
  fix16_to_float(pru_mem->p->thr)
  );
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  float input_float = 0;
  char input_string[256] = {0};
  char log_file[256] = "/root/openWearable/apps/datalog/";

  UiPrintMenu(pru_mem);
  while (1) {
    // Clear inputs
    input_char = ' ';
    input_string[0] = '\0';

    if (UiGetPruCtlBit(pru_mem, 1)) {
      UiStopAndSaveLog();
      printf("done.\n");
      return 1;
    }

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
        case 'f' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start trial...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          // Wait for enter to stop collection
          printf("\t\tTrial ongoing...\n");
          fflush(stdout);

          // Data collection loop
          UiStartLog();
          UiSetPruCtlBit(pru_mem, 0);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Stop data collection -----------------------------------------
        case 's' : {
          if (!UiLogging()) {
            printf("\t\t Not currently logging data!\n");
          } else {
            UiClearPruCtlBit(pru_mem, 0);
            UiStopAndSaveLog();
          }

          log_file[0] = '\0';
          strcat(log_file, "/root/openWearable/apps/datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change parameter ----------------------------------------------
        case 'p' : {
          printf("\t\tChoose a parameter to change\n");
          UiParamMenu();
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %c", &input_char);

          switch (input_char) {
            case 'a' : {
              printf("\t\tEnter new value: ");
              fflush(stdout);
              UiPollForUserInput();
              scanf(" %f", &input_float);
              pru_mem->p->p_0 = fix16_from_float(input_float);
              break;
            }
            case 's' : {
              printf("\t\tEnter new value: ");
              fflush(stdout);
              UiPollForUserInput();
              scanf(" %f", &input_float);
              pru_mem->p->dp = fix16_from_float(input_float);
              break;
            }
            case 'd' : {
              printf("\t\tEnter new value: ");
              fflush(stdout);
              UiPollForUserInput();
              scanf(" %f", &input_float);
              pru_mem->p->thr = fix16_from_float(input_float);
              break;
            }
          }
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

