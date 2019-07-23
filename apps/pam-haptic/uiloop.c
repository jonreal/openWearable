#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t fd = %3.2f,\t Np = %i,\t Pd = %3.2f\n\n"
  "Menu: f - Start trial\n"
  "      p - change target frequency\n"
  "      n - change number of cycles\n"
  "      d - change pam resting pressure\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  (float)pru_mem->p->fs_hz/(float)pru_mem->p->Td,
  pru_mem->p->Np,
  fix16_to_float(pru_mem->p->pd));
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  int input_int = 0;
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

          // Poll for PRU done
          UiPollPruCtlBit(pru_mem, 0, 0);
          UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change frequency ----------------------------------------------
        case 'p' : {
          printf("\t\tEnter new target frequency (Hz): ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Td = (uint32_t)((float)pru_mem->p->fs_hz/input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change cycels -------------------------------------------------
        case 'n' : {
          printf("\t\tEnter new cycle count: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %u", &input_int);
          pru_mem->p->Np = (uint32_t)input_int;
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

