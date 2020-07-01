#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;
extern volatile sig_atomic_t sigexit;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: s - start log\n"
  "      d - stop log\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  UiPrintMenu(pru_mem);
  while (1) {
    // Clear inputs
    input_char = ' ';
    input_string[0] = '\0';

    if (sigexit) {
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
          UiStartLog();

          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Stop data collection -----------------------------------------
        case 'd' : {
          if (!UiLogging())
            printf("\t\t Not currently logging data!\n");
          else
            UiStopAndSaveLog();

					if (sigexit)
						return 1;

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}


// ----------------------------------------------------------------------------
int PruLoadParams(const char* file, param_mem_t* param) {

  // Defaults
  param->fs_hz = 1000;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);

  return 0;
}

