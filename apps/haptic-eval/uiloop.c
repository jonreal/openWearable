#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t Jv = %3.2f,\t Bv = %3.2f,\t Kv = %3.7f\n"
  " \t fd = %3.2f,\t Np = %i\n"
  " \t P0 = %3.2f,\t dP = %3.2f,\t threshold = %3.5f\n\n"
  "Menu: t - start tracking experiment\n"
  "      r - start ballistic experiment\n"
  "      p - change target frequency\n"
  "      n - change number of cycles\n"
  "      b - change damping\n"
  "      k - change stiffnes\n"
  "      j - change inertia\n"
  "      h - change P0\n"
  "      g - change dP\n"
  "      f - change threshold\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->Jvirtual),
  fix16_to_float(pru_mem->p->bvirtual),
  fix16_to_float(pru_mem->p->kvirtual),
  (float)pru_mem->p->fs_hz/(float)pru_mem->p->Td,
  pru_mem->p->Np,
  fix16_to_float(pru_mem->p->P0),
  fix16_to_float(pru_mem->p->dP),
  fix16_to_float(pru_mem->p->threshold));
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

        // ---- Collect trial -------------------------------------------------
        case 'r' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          UiNewLogFile(log_file);

          for (int i=0; i<20; i++) {
            // random target between -170 and 170
            pru_mem->p->targets[i] = fix16_sdiv(
                                  fix16_from_int((rand() % 1600) - 800),
                                  fix16_from_int(10));
           // printf("%f\n", fix16_to_float(pru_mem->p->targets[i]));
          }

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
          UiSetPruCtlBit(pru_mem, 1);

          // Poll for PRU done
          UiPollPruCtlBit(pru_mem, 1, 0);
          UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }



        // ---- change time -------------------------------------------------
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

        // ---- change time -------------------------------------------------
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

        // ---- change damping -------------------------------------------------
        case 'b' : {
          printf("\t\tEnter damping factor: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->bvirtual = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change damping -------------------------------------------------
        case 'k' : {
          printf("\t\tEnter stiffness factor: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->kvirtual = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change damping -------------------------------------------------
        case 'j' : {
          printf("\t\tEnter virtual inertia: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Jvirtual = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change P0 -------------------------------------------------
        case 'h' : {
          printf("\t\tEnter new P0: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->P0 = fix16_from_float(input_float);
          UiSetPruCtlBit(pru_mem,2);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change dP -------------------------------------------------
        case 'g' : {
          printf("\t\tEnter new dP: ");
          fflush(stdout);

          // Wait for input.
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->dP = fix16_from_float(input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change threshold -------------------------------------------------
        case 'f' : {
          printf("\t\tEnter threshold: ");
          fflush(stdout);

          // Wait for input.
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

