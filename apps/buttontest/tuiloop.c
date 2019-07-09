#include "tui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void TuiPrintMenu(pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " pd_sup = %.2f\t pd_pro = %.2f\t t_hold = %.2f\n\n"
  "Menu: f - Collect trial\n"
  "      t - change t_hold\n"
  "      s - change pd_sup\n"
  "      p - change pd_pro\n"
  "      e - exit\n"
  "---------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->pd_sup),
  fix16_to_float(pru_mem->p->pd_pro),
  (float)pru_mem->p->hold_cnt/1000.0);
  fflush(stdout);
}

int TuiLoop(pru_mem_t* pru_mem) {
  char input_char = 0;
  int input_int = 0;
  float input_float = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  TuiInitLogAndPublishThread(pru_mem);
  TuiPrintMenu(pru_mem);

  while (1) {
    // Clear inputs
    input_float = 0;
    input_int = 0;
    input_char = ' ';
    input_string[0] = '\0';

    // Wait for user input.
    if (input_ready) {
      scanf(" %c", &input_char);

      switch(input_char){

        // ---- Exit ----------------------------------------------------------
        case 'e' : {
          TuiCloseLogAndPublishThread();
          printf("done.\n");
          return 1;
        }

        // ---- Collect trial -------------------------------------------------
        case 'f' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          TuiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          TuiPollForUserInput();
          TuiSetPruCtlBit(pru_mem, 1); // sync w bit 1 in pru_ctl utility
          scanf(" %c", &input_char);

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);

          // Data collection loop
          TuiStartLog();
          TuiPollForUserInput();
          TuiStopAndSaveLog();
          scanf(" %c", &input_char);
          TuiClearPruCtlBit(pru_mem, 1); // sync w bit 1 in pru_ctl utility

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          TuiPrintMenu(pru_mem);
          break;
        }

        // ---- change time -------------------------------------------------
        case 't' : {
          printf("\t\tEnter new hold time (in sec): ");
          fflush(stdout);

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->hold_cnt = input_float*1000;
          TuiPrintMenu(pru_mem);
          break;
        }

        // ---- change pd_sup -------------------------------------------------
        case 's' : {
          printf("\t\tEnter new pd_sup (in psi): ");
          fflush(stdout);

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->pd_sup = fix16_from_float(input_float);
          TuiPrintMenu(pru_mem);
          break;
        }

        // ---- change pd_pro -------------------------------------------------
        case 'p' : {
          printf("\t\tEnter new pd_pro (in psi): ");
          fflush(stdout);

          // Wait for input.
          TuiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->pd_pro = fix16_from_float(input_float);
          TuiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

