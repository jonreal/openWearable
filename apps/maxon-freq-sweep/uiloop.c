#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t Tf = %3.2f,\t f0 = %3.2f,\t f1 = %3.2f,\t A = %3.2f\n\n"
  "Menu: f - Start trial\n"
  "      p - change total time\n"
  "      o - change initial frequency\n"
  "      i - change final frequency\n"
  "      u - change amplitude\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  (float)pru_mem->p->Tf/(float)pru_mem->p->fs_hz,
  fix16_to_float(pru_mem->p->f0),
  fix16_to_float(pru_mem->p->f1),
  fix16_to_float(pru_mem->p->A));
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

        // ---- change time -------------------------------------------------
        case 'p' : {
          printf("\t\tEnter new total time: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Tf = (uint32_t)((float)pru_mem->p->fs_hz*input_float);
          UiSetPruCtlBit(pru_mem,2);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- start freq -------------------------------------------------
        case 'o' : {
          printf("\t\tEnter new start frequency: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->f0 = fix16_from_float(input_float);
          UiSetPruCtlBit(pru_mem,2);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- end freq -------------------------------------------------
        case 'i' : {
          printf("\t\tEnter new end frequency: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->f1 = fix16_from_float(input_float);
          UiSetPruCtlBit(pru_mem,2);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- amplitude -------------------------------------------------
        case 'u' : {
          printf("\t\tEnter new amplitude: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->A = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

