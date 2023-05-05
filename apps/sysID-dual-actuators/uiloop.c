#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t f0 = %3.2f,\t f1 = %3.2f,\t A = %3.2f,\n"
  " \t Pd1 = %3.2f,\t Pd2 = %3.2f,\t Step = %3.2f, Repeat = %3.2f \n\n"
  "Menu: s - Start trial\n"
  "      i - change initial frequency\n"
  "      f - change final frequency\n"
  "      a - change amplitude\n"
  "      m - change desired pressure 1\n"
  "      n - change desired pressure 2\n"
  "      c - change frequency step\n"
  "      r - change repetition\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->f0),
  fix16_to_float(pru_mem->p->f1),
  fix16_to_float(pru_mem->p->A),
  fix16_to_float(pru_mem->p->Pd1),
  fix16_to_float(pru_mem->p->Pd2),
  fix16_to_float(pru_mem->p->step),
  fix16_to_float(pru_mem->p->repeat));
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

        // ---- start freq -------------------------------------------------
        case 'i' : {
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
        case 'f' : {
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
        case 'a' : {
          printf("\t\tEnter new amplitude: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->A = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Pd1 -------------------------------------------------
        case 'm' : {
          printf("\t\tEnter new desired pressure 1: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Pd1 = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Pd2 -------------------------------------------------
        case 'n' : {
          printf("\t\tEnter new desired pressure 2: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Pd2 = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change cycle -------------------------------------------------
        case 'c' : {
          printf("\t\tEnter new frequency step: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->step = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change repatition -------------------------------------------------
        case 'r' : {
          printf("\t\tEnter new repetiton: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->repeat = fix16_from_float(input_float);
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
//  param->Tf = 20000;
  param->f0 = 0x1999;
  param->f1 = fix16_one;
  param->A = 0x10000;
  param->Pd1 = 0x1E0000;
  param->Pd2 = 0x1E0000;
  param->step = 0x00CCC;
  param->repeat = 0xA0000;

  return 0;
}
