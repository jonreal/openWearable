#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;
extern volatile sig_atomic_t sigexit;

typedef enum {
  TRACKING,
  BALLISTIC,
} game_t;

typedef struct {
  game_t game;
  fix16_t Jv;
  fix16_t Bv;
  fix16_t Kv;
  fix16_t P0;
  fix16_t dP;
} conditions_t;

uint32_t nCond = 3;
uint32_t nBlocks = 5;
uint32_t random_index[15] = {1, 2, 3,
                             3, 1, 2,
                             1, 3, 2,
                             2, 1, 3,
                             3, 2, 1};

conditions_t conditions[3] = { {.game = TRACKING,
                                .Jv = 0,
                                .Kv = 0,
                                .P0 = 0,
                                .dP = 0},
                               {.game = TRACKING,
                                .Jv = 0x50000,
                                .Kv = 0,
                                .P0 = 0,
                                .dP = 0},
                               {.game = TRACKING,
                                .Jv = 0,
                                .Kv = 0x7AE,
                                .P0 = 0,
                                .dP = 0}};

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: s - start experiment\n"
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

          for (int j=0; j<nBlocks; j++) {
            for (int i=0; i<nCond; i++) {
              sprintf(input_string, "block%i-trial%i-J%2.3f-K%2.4f",
                      j, i, fix16_to_float(conditions[i].Jv),
                      fix16_to_float(conditions[i].Kv));
              strcat(log_file, input_string);
              UiNewLogFile(log_file);

              pru_mem->p->Jvirtual = conditions[i].Jv;
              pru_mem->p->kvirtual = conditions[i].Kv;

              // Wait for user input to start saving data
              printf("\t\tPress enter start block%i trial%i,...\n",(j+1),(i+1));
              fflush(stdout);
              UiPollForUserInput();
              scanf(" %c", &input_char);

              printf("\t\ttrial ongoing...\n");
              // Data collection loop
              UiStartLog();
              UiSetPruCtlBit(pru_mem, 0);

              // Poll for PRU done
              UiPollPruCtlBit(pru_mem, 0, 0);
              UiStopAndSaveLog();

              log_file[0] = '\0';
              strcat(log_file, "datalog/");
            }
            if (sigexit)
              break;
          }
          break;
        }
      }
    }
  }
}

