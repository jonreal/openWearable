#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Parameters: \t Pmax = %3.2f,\t stepwidth = %i,\t dP = %3.7f\n\n"
  "Menu: f - Collect trial\n"
  "      a - change Pmax\n"
  "      s - change stepwidth\n"
  "      d - change dP\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->Pmax),
  pru_mem->p->stepwidth,
  fix16_to_float(pru_mem->p->dP));
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";
  float input_float = 0;

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

        // ---- Collect trial -------------------------------------------------
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

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);

          // Data collection loop
          printf("\t\tStarecase response ongoing...\n");
          UiStartLog();
          UiSetPruCtlBit(pru_mem, 0);

          UiPollPruCtlBit(pru_mem, 0, 0);
          UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Pmax -------------------------------------------------
        case 'a' : {
          printf("\t\tEnter new Pmax: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Pmax = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change stepwidth -------------------------------------------------
        case 's' : {
          printf("\t\tEnter new stepwidth (ms): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->stepwidth = (uint32_t)((float)input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change stepwidth -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter new dP: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->dP = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}


int PruLoadParams(const char* file, param_mem_t* param) {

  // Defaults
  param->fs_hz = 1000;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);

  // App specific
  param->Pmax = 0x500000;
  param->stepwidth = 2000;
  param->dP = 0x50000;

  return 0;
}
