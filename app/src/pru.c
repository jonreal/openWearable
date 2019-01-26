/* Copyright 2018 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include "pru.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "mem_types.h"

// ---------------------------------------------------------------------------
// Function: int PruMemMap(pru_mem_t)
//
//  This function sets pointers to pru0RAM, pru1RAM, sharedRAM
//
// Inputs:    pru_mem_t* - pointer to pru_mem pointer struct
// Outputs:   returns 0 on success
// ---------------------------------------------------------------------------
int PruMemMap(pru_mem_t* pru_mem) {
  int fd = open("/dev/mem", O_RDWR | O_SYNC);

  // Memory Map for param (pru0 DRAM)
  void* ptr = mmap(0, sizeof(param_mem_t),
                   PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU0_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  pru_mem->p = (param_mem_t*) ptr;

  // Memory Map for lookup table (pru1 DRAM)
  ptr = mmap(0, sizeof(lut_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU1_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  pru_mem->l = (lut_mem_t *) ptr;

  // Memory Map for state (shared DRAM)
  ptr = mmap(0, sizeof(shared_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_SHARED_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  pru_mem->s = (shared_mem_t *) ptr;
  close(fd);

  // Zero State
  for (int i=0; i<STATE_BUFF_LEN; i++)
    memset(&(pru_mem->s->state[i]), 0, sizeof(state_t));
  for (int i=0; i<10; i++){
    pru_mem->p->debug_buff[i] = 0;
  }

  PruCtlReset(&pru_mem->s->pru_ctl);

  char buff[1024] = {0,};
  PruSprintMalloc(pru_mem, buff);
  fprintf(stdout, buff);
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int PruInit(void)
//
//  This function restarts and loads pru firmware (rproc).
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int PruInit(void){
  if (PruRestart() == -1) {
    printf("restart pru failed.\n");
    return -1;
  }
  // open file descriptors for pru rproc driver
  int bind_fd = open(PRU_BIND, O_WRONLY);
  if (bind_fd == -1) {
    printf("ERROR: pru-rproc driver missing\n");
    return -1;
  }
  // if pru0 is not loaded, load it
  if (access(PRU0_UEVENT, F_OK)!=0) {
    if (write(bind_fd, PRU0_NAME, PRU_NAME_LEN)<0) {
      printf("ERROR: pru0 bind failed\n");
      return -1;
    }
  }
  // if pru1 is not loaded, load it
  if (access(PRU1_UEVENT, F_OK)!=0) {
    if (write(bind_fd, PRU1_NAME, PRU_NAME_LEN)<0) {
      printf("ERROR: pru1 bind failed\n");
      return -1;
    }
  }
  close(bind_fd);
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int PruRestart(void)
//
//  This function restarts pru cores.
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int PruRestart(void) {
  // open file descriptors for pru rproc driver
  int unbind_fd = open(PRU_UNBIND, O_WRONLY);
  if (unbind_fd == -1) {
    printf("open unbind fail\n");
    return -1;
  }
  int bind_fd = open(PRU_BIND, O_WRONLY);
  if (bind_fd == -1) {
    printf("open bind fail\n");
    return -1;
  }
  // if pru0 is loaded, unload it
  if (access(PRU0_UEVENT, F_OK)==0) {
    if (write(unbind_fd, PRU0_NAME, PRU_NAME_LEN)<0) {
      printf("ERROR: pru0 unbind failed\n");
      return -1;
    }
  }
  // if pru1 is loaded, unload it
  if (access(PRU1_UEVENT, F_OK)==0) {
    if (write(unbind_fd, PRU1_NAME, PRU_NAME_LEN)<0) {
      printf("ERROR: pru1 unbind failed\n");
      return -1;
    }
  }
  // now bind both
  if (write(bind_fd, PRU0_NAME, PRU_NAME_LEN)<0) {
    printf("ERROR: pru0 bind failed\n");
    return -1;
  }
  if (write(bind_fd, PRU1_NAME, PRU_NAME_LEN)<0) {
    printf("ERROR: pru1 bind failed\n");
    return -1;
  }
  close(unbind_fd);
  close(bind_fd);
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int PruCleanup(void)
//
// Restarts prus.
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int PruCleanup(void) {
  if (PruRestart() == -1) {
    printf("restart pru failed.\n");
    return -1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// Function: void PruCtlReset(void)
//
// This function clears flow bit feild used by prus.
//
// Input:   pointer to pru control stuct (pru_ctl_t).
// ---------------------------------------------------------------------------
void PruCtlReset(pru_ctl_t* ctl) {
  ctl->reg = 0x00;
}

// ---------------------------------------------------------------------------
// Function: void PruSprintMemoryAllocation(const pru_mem_t* pru_mem,
//                                          char* buff)
//
//  This function prints the size of each chunk of memory, eg, shared memory,
//  parameter memory and lookup table. The second function stores the string
//  in a buff.
//
//  Input: pointer to buff
// ---------------------------------------------------------------------------
void PruSprintMalloc(const pru_mem_t* pru_mem, char* buff) {
  sprintf(buff,
          "#\n#Memory Allocation:\n"
          "#\tParameter memory: %i bytes.\n"
          "#\tLookup table memory: %i bytes.\n"
          "#\tData memory: %i bytes.\n#",
          sizeof(*(pru_mem->p)),
          sizeof(*(pru_mem->l)),
          sizeof(*(pru_mem->s)));
}

// ----------------------------------------------------------------------------
// Functions: void PruLoadParams(char* file)
//
// This function loads parameters from file.
//
// Input:   path to file
//
// TODO: Use strcmp for parameter inputs
// ------------------------------------------------------------------------- */
int PruLoadParams(const char* file, param_mem_t* param) {
  FILE* fp = fopen(file, "r");
  if (fp != NULL) {
    fscanf(fp, "%u%*[^\n]\n", &param->fs_hz);
    fclose(fp);
    param->fs_ticks = HZ_TO_TICKS(param->fs_hz);
    return 0;
  }
  return -1;
}

void PruSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n#",
          param->fs_hz);
}

// ---------------------------------------------------------------------------
// Function: void printState(uint8_t si, FILE *fp)
//           void sprintState(uint8_t si, char* buff)
//
//  This function prints the states.
//
// Input: pointer to file or buff
// ---------------------------------------------------------------------------
void PruSprintState(const state_t* st, char* buff) {
  sprintf(buff,
          "%u\t"        // timeStamp - uint32_t
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // p1_m - fix16_t
          "%f\t"        // dp1_m - fix16_t
          "%f\t"        // p1_d - fix16_t
          "%f\t"        // p2_m - fix16_t
          "%f\t"        // dp2_m - fix16_t
          "%f\t"        // p2_d - fix16_t
          "%i\t"        // u1 - int16_t
          "%i\t"        // u2 - int16_t
          "%f\t"        // angle - fix16_t
          "%f\t"        // angle_d - fix16_t
          "%u\t"        // sync_val - uint16_t
          "\n",
          st->time,
          fix16_to_float(st->emg1_raw),
          fix16_to_float(st->emg1_rect),
          fix16_to_float(st->emg1_nlb),
          fix16_to_float(st->emg2_raw),
          fix16_to_float(st->emg2_rect),
          fix16_to_float(st->emg2_nlb),
          fix16_to_float(st->p1_m),
          fix16_to_float(st->dp1_m),
          fix16_to_float(st->p1_d),
          fix16_to_float(st->p2_m),
          fix16_to_float(st->dp2_m),
          fix16_to_float(st->p2_d),
          (int32_t)st->u1,
          (int32_t)st->u2,
          fix16_to_float(st->angle),
          fix16_to_float(st->angle_d),
          st->sync_val
          );
}

void PruSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "emg1_raw\t"
          "emg1_rect\t"
          "emg1_nlb\t"
          "emg2_raw\t"
          "emg2_rect\t"
          "emg2_nlb\t"
          "p1_m\t"
          "dp1_m\t"
          "p1_d\t"
          "p2_m\t"
          "dp2_m\t"
          "p2_d\t"
          "u1\t"
          "u2\t"
          "angle\t"
          "angle_d\t"
          "sync_val\t"
          "\n");
}



// ----------------------------------------------------------------------------
// Function: void PruEnable(int en, pru_ctl_t* ctl)
//
//  This function sets the enbable bit in the flow bit feild to 1 (enable).
// ---------------------------------------------------------------------------
void PruEnable(int en, pru_ctl_t* ctl) {
  if (en == 1)
    ctl->bit.enable = 1;
  else
    ctl->bit.enable = 0;
}

// ---------------------------------------------------------------------------
// Function: void printDebugBuffer(void)
//
//  This function prints the debug buff.
// ---------------------------------------------------------------------------
void PruPrintDebugBuffer(const volatile uint32_t* db) {
  printf("\n\n---- Debug Buffer ----\n");
  for (int i=0; i<10; i++) {
    printf("0x%X \t%i\n", db[i], db[i]);
  }
}


/* ----------------------------------------------------------------------------
 * Functions: void loadLookUpTable(char* file)
 *
 * This function loads lookup table (Feedforward) from file to memory.
 * ------------------------------------------------------------------------- */
int PruLoadLut(char* file, lut_mem_t* l)
{
  FILE* fp = fopen(file, "r");
  float value;

  if(fp != NULL){
    for(int i=0; i<1000; i++){
      fscanf(fp, "%f\n", &value);

      // Scale signal by 1000 to store as int16_t
      l->lut[i] = (int16_t) fix16_to_int(fix16_from_float(value * 1000.0));
    }
    fclose(fp);
    return 0;
  }
  printf("File not found\n");
  return -1;
}
