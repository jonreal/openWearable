#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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
  fix16_t G;
  uint32_t reflex_condition;
} conditions_t;




//uint32_t nBlocks = 5;
//uint32_t nCond = 4;
//uint32_t random_index[20];
//conditions_t conditions[4] = { {.game = TRACKING,
//                                .Jv = 0,                  // 0
//                                .Kv = 0,                  // 0
//                                .P0 = 0,                  // 0
//                                .G = 0,                   // 0
//                                .reflex_condition = 0},   // 0
//                                {.game = TRACKING,
//                                .Jv = 0,                  // 0
//                                .Kv = 0x7AE,              // 0.03
//                                .P0 = 0,                  // 0
//                                .G = 0,                   // 0
//                                .reflex_condition = 0},   // 0
//                              {.game = TRACKING,
//                                .Jv = 0x50000,            // 5
//                                .Kv = 0,                  // 0
//                                .P0 = 0,                  // 0
//                                .G = 0,                   // 0
//                                .reflex_condition = 0},   // 0
//                              {.game = TRACKING,
//                                .Jv = 0x50000,            // 5
//                                .Kv = 0xCCD,              // 0.05
//                                .P0 = 0,                  // 0
//                                .G = fix16_one,           // 1
//                                .reflex_condition = 0}    // 0
//                               };






uint32_t nCond = 12;
uint32_t nBlocks = 5;
uint32_t random_index[60];
conditions_t conditions[12] = { {.game = TRACKING,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0,                  // 0
                                .G = 0,                   // 0
                                .reflex_condition = 0},   // 0
                                {.game = TRACKING,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0,                  // 0
                                .G = 0,                   // 0
                                .reflex_condition = 0},   // 0
                                {.game = BALLISTIC,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0,                  // 0
                                .G = 0,                   // 0
                                .reflex_condition = 0},   // 0
                                {.game = BALLISTIC,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0,                  // 0
                                .G = 0,                   // 0
                                .reflex_condition = 0},   // 0
                                ////////////////////////////
                                {.game = TRACKING,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 1},   // myo
                                {.game = TRACKING,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 1},   // myo
                                {.game = BALLISTIC,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 1},   // myo
                                {.game = BALLISTIC,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 1},    // myo
                                ////////////////////////////
                                {.game = TRACKING,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 3},   // myo + reflex
                                {.game = TRACKING,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 3},   // myo + reflex
                                {.game = BALLISTIC,
                                .Jv = 0,                  // 0
                                .Kv = 0x51F,              // 0.03
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 3},   // myo + reflex
                                {.game = BALLISTIC,
                                .Jv = 0x40000,            // 4
                                .Kv = 0,                  // 0
                                .P0 = 0x280000,           // 40 psi
                                .G = 0,                   // 0
                                .reflex_condition = 3}    // myo + reflex
                              };



static void my_sleep(unsigned duration)
{
    time_t start = time(NULL);
    double end = duration;
    time_t now;
    do {
        now = time(NULL);
    } while (difftime(now, start) < end);
}

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Menu: s - start experiment\n"
  "      m - collect mvc\n"
  "      f - collect free play\n"
  "      p - parameter menu\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
}

static void UiPrintParamMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  " \t dP = %.2f,\t threshold = %.3f,\n"
  " \t emg_up_thres = %.3f,\t emg_low_thres = %.3f\n"
  "Parameters: d - change dP\n"
  "            f - change threshold\n"
  "            g - change emg_up_thres\n"
  "            h - change emg_low_thres\n"
  "            a - done\n"
  "-----------------------------------------------------------------------\n",
  fix16_to_float(pru_mem->p->dP),
  fix16_to_float(pru_mem->p->threshold),
  fix16_to_float(pru_mem->p->emg_up_thres),
  fix16_to_float(pru_mem->p->emg_low_thres)
  );
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  float input_float = 0.0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";


  // make sample back with each condition
  // first nCond should be in order
  uint32_t sample_bag[nCond];
  for (int i=0; i<nCond; i++) {
    sample_bag[i] = i;
    random_index[i] = i;
  }

  // in each block conditions are randomized
  uint32_t block_bag[nCond];
  uint32_t randi;
  for (int i=1; i<nBlocks; i++) {
    memcpy(block_bag, sample_bag, sizeof(sample_bag));
    for (int j=0; j<(nCond); j++) {
      randi = rand() % (nCond-j);
      random_index[i*nCond + j] = block_bag[randi];
      for(int k=randi+1; k<nCond; k++){
        block_bag[k-1] = block_bag[k];
      }
    }
  }

  FILE* ff = fopen("datalog/condition-key","w");
  for (int j=0; j<nBlocks; j++) {
    for (int i=0; i<nCond; i++){
      fprintf(ff,"%i.%i - %i\n",(j+1),(i+1),(random_index[nCond*j+i]+1));
    }
    fprintf(ff,"\n");
  }
  fclose(ff);

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


        // ---- Exit ----------------------------------------------------------
        case 'p' : {
          int loopdone = 0;
          while(!loopdone) {
            UiPrintParamMenu(pru_mem);
            fflush(stdout);
            // Wait for input.
            UiPollForUserInput();
            scanf(" %c", &input_char);

            switch (input_char) {
              case 'd' : {
                printf("\t\tEnter new value: ");
                fflush(stdout);
                UiPollForUserInput();
                scanf(" %f", &input_float);
                pru_mem->p->dP = fix16_from_float(input_float);
                break;
              }
              case 'f' : {
                printf("\t\tEnter new value: ");
                fflush(stdout);
                UiPollForUserInput();
                scanf(" %f", &input_float);
                pru_mem->p->threshold = fix16_from_float(input_float);
                break;
              }
              case 'g' : {
                printf("\t\tEnter new value: ");
                fflush(stdout);
                UiPollForUserInput();
                scanf(" %f", &input_float);
                pru_mem->p->emg_up_thres = fix16_from_float(input_float);
                break;
              }
              case 'h' : {
                printf("\t\tEnter new value: ");
                fflush(stdout);
                UiPollForUserInput();
                scanf(" %f", &input_float);
                pru_mem->p->emg_low_thres = fix16_from_float(input_float);
                break;
              }
              case 'a' : {
                loopdone = 1;
                break;
              }
            }
          }
          UiPrintMenu(pru_mem);
          break;
        }



        // ---- Start data collection -----------------------------------------
        case 's' : {

          uint32_t trialcnt = 0;
          uint32_t icond = 0;
          for (int j=0; j<nBlocks; j++) {
            for (int i=0; i<nCond; i++) {

              ////Add here to skip trials
              //if ((j == 0) && (i < 5)) {
              //  trialcnt++;
              //  continue;
              //}

              icond = random_index[trialcnt];
              sprintf(input_string,
                  "block%i-trial%i",(j+1), (i+1));

              int k=0;
              while (input_string[k]!=0) {
                if (input_string[k] == '.' ) {
                  input_string[k] = '_';
                }
                k++;
              }
              strcat(log_file, input_string);
              UiNewLogFile(log_file);

              // Wait for user input to start saving data
              printf("\t\tPress enter start block%i trial%i,...\n",(j+1),(i+1));
              fflush(stdout);
              UiPollForUserInput();
              scanf(" %c", &input_char);

              pru_mem->p->Jvirtual = conditions[icond].Jv;
              pru_mem->p->kvirtual = conditions[icond].Kv;
              pru_mem->p->P0 = conditions[icond].P0;
              pru_mem->p->G = conditions[icond].G;
              pru_mem->p->reflex_condition = conditions[icond].reflex_condition;


              pru_mem->p->targets[0] = fix16_sdiv(
                                    fix16_from_int((rand() % 1600) - 800),
                                    fix16_from_int(10));


              if (conditions[icond].game == BALLISTIC) {
                for (int i=1; i<20; i++) {
                  // random target between -170 and 170
                  while (1)  {
                    pru_mem->p->targets[i] = fix16_sdiv(
                                        fix16_from_int((rand() % 1600) - 800),
                                        fix16_from_int(10));
                    fix16_t diff =
                      fix16_ssub(pru_mem->p->targets[i-1],
                                 pru_mem->p->targets[i]);
                    if ((fix16_ssub(diff,fix16_from_int(20)) > 0)
                      || (fix16_sadd(diff,fix16_from_int(20)) < 0)) {
                      break;
                    }
                  }
                }
              }

              UiSetPruCtlBit(pru_mem,2);
							my_sleep(2);

              printf("\t\ttrial ongoing...\n");

              // Data collection loop
              UiStartLog();

              switch (conditions[icond].game)  {
                case TRACKING:
                  UiSetPruCtlBit(pru_mem, 0);
                  UiPollPruCtlBit(pru_mem, 0, 0);
                  break;
                case BALLISTIC:
                  UiSetPruCtlBit(pru_mem, 1);
                  UiPollPruCtlBit(pru_mem, 1, 0);
                  break;
              }

              // Poll for PRU done
              UiStopAndSaveLog();

              if (sigexit)
                return 1;

							my_sleep(2);
              pru_mem->p->Jvirtual = 0;
              pru_mem->p->kvirtual = 0;
              pru_mem->p->P0 = 0;
              UiSetPruCtlBit(pru_mem,2);

              log_file[0] = '\0';
              input_string[0] = '\0';
              strcat(log_file, "datalog/");
              trialcnt++;
            }
          }
          printf("done.\n");
          return 1;
        }

        // ---- Start data collection -----------------------------------------
        case 'm' : {


          sprintf(input_string,"mvc");
          strcat(log_file, input_string);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start MVC collection...\n");
          fflush(stdout);

          UiPollForUserInput();
          scanf(" %c", &input_char);

          pru_mem->p->P0 = 0;
          UiSetPruCtlBit(pru_mem,2);
					my_sleep(2);

          // Data collection loop
          UiStartLog();

          printf("\t\tPress enter to stop MVC collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          UiStopAndSaveLog();

          pru_mem->p->Jvirtual = 0;
          pru_mem->p->kvirtual = 0;
          pru_mem->p->P0 = 0;
          UiSetPruCtlBit(pru_mem,2);
					my_sleep(2);

          input_string[0] = '\0';
          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Start data collection -----------------------------------------
        case 'f' : {


          sprintf(input_string,"freeplay");
          strcat(log_file, input_string);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start free play collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          pru_mem->p->P0 =  0x280000;
          pru_mem->p->reflex_condition = 3;
          UiSetPruCtlBit(pru_mem,2);
					my_sleep(2);

          // Data collection loop
          UiStartLog();

          printf("\t\tPress enter to stop free play collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          UiStopAndSaveLog();

          pru_mem->p->Jvirtual = 0;
          pru_mem->p->kvirtual = 0;
          pru_mem->p->P0 = 0;
          pru_mem->p->reflex_condition = 0;
          UiSetPruCtlBit(pru_mem,2);
					my_sleep(2);

          input_string[0] = '\0';
          log_file[0] = '\0';
          strcat(log_file, "datalog/");
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
  param->Jvirtual = 0;
  param->bvirtual = 0;
  param->kvirtual = 0;

  param->P0 = 0;

  // patient specific
  param->threshold = fix16_from_float(0.1);
  param->dP = fix16_from_int(8);
  param->emg_up_thres = fix16_from_int(1);
  param->emg_low_thres = -fix16_from_int(1);

  param->reflex_condition = 0;
  param->Nb = 20;
  param->Np = fix16_from_int(15);
  param->fd = fix16_from_float(0.2);

  param->Td = fix16_sdiv(param->Np,param->fd);
  param->k2PiFd = fix16_smul(fix16_smul(0x20000,fix16_pi),param->fd);


  FILE* fp = fopen(file, "r");
  float infloat;
  if (fp != NULL) {
    fscanf(fp, "%f%*[^\n]\n", &infloat);
    param->dP = fix16_from_float(infloat);

    fscanf(fp, "%f%*[^\n]\n", &infloat);
    param->threshold = fix16_from_float(infloat);

    fscanf(fp, "%f%*[^\n]\n", &infloat);
    param->emg_up_thres = fix16_from_float(infloat);

    fscanf(fp, "%f%*[^\n]\n", &infloat);
    param->emg_low_thres = fix16_from_float(infloat);

    fclose(fp);
    return 0;
  }
  return 0;
}
