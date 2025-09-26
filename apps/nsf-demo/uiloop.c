#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t P0 = %3.2f,\t dP = %3.2f,\t threshold = %3.5f\n\n"
  "Menu: s - start log\n"
  "      d - stop log\n"
  "      h - change P0\n"
  "      g - change dP\n"
  "      f - change threshold\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->P0),
  fix16_to_float(pru_mem->p->dP),
  fix16_to_float(pru_mem->p->threshold));
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  float input_float = 0.0;
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

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // --- P0
        case 'h' : {
          printf("\t\tEnter new value: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->P0 = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          UiSetPruCtlBit(pru_mem,2);
          break;
        }

        // --- dP
        case 'g' : {
          printf("\t\tEnter new value: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->dP = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // --- threshold
        case 'f' : {
          printf("\t\tEnter new value: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->threshold = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

int PruLoadParams(const char* file, param_mem_t* param) {

  // Defaults
  param->fs_hz = 800;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);

  param->P0 = 0;
  param->threshold = fix16_from_float(0.025);
  param->dP = fix16_from_int(4);

  return 0;
}


