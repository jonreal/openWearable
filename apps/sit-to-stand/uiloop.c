#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Parameters:\tPtarget = %3.2f,\tTdelay = %i,\tTtrial = %i\n\n"
  "Menu: t - collect trial\n"
  "      a - change Ptarget\n"
  "      s - change Tdelay\n"
  "      d - change Ttrial\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->Ptarget),
  pru_mem->p->Tdelay,
  pru_mem->p->Ttrial);
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

        // ---- Start data collection -----------------------------------------
        case 't' : {
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

          // Data collection loop
          printf("\t\tTrial collection ongoing...\n");
          UiStartLog();
          UiSetPruCtlBit(pru_mem, 0);

          UiPollPruCtlBit(pru_mem, 0, 0);
          UiStopAndSaveLog();

          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Ptarget -------------------------------------------------
        case 'a' : {
          printf("\t\tEnter new Ptarget: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Ptarget = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Tdelay -------------------------------------------------
        case 's' : {
          printf("\t\tEnter new Tdelay (ms): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Tdelay = (uint32_t)((float)input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change stepwidth -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter new Ttrial (ms): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Ttrial = (uint32_t)((float)input_float);
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

  param->Ptarget = fix16_from_float(15.0);
  param->Tdelay = 10000;
  param->Ttrial = 30000;

  return 0;
}


