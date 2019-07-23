#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \tPd = %3.2f\n\n"
  "Menu: f - start log\n"
  "      g - stop log\n"
  "      d - change pam resting pressure\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->pd));
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
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          // Data collection loop
          UiStartLog();

          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Stop data collection -----------------------------------------
        case 'g' : {
          if (!UiLogging())
            printf("\t\t Not currently logging data!\n");
          else
            UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change resting pressure ---------------------------------------
        case 'd' : {
          printf("\t\tEnter new resting pressure: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->pd = fix16_from_float(input_float);
          UiSetPruCtlBit(pru_mem,1);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

