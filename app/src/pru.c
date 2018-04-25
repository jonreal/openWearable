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
#include"mem_types.h"

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
  ptr = mmap(0, sizeof(lookUp_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU1_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  pru_mem->l = (lookUp_mem_t *) ptr;

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

// ----------------------------------------------------------------------------
// Functions: void loadIirFilterCoeff(char* file)
//
// This function loads filter coeff from file to memory.
//
// file format:
//    FilterOrder
//    b(1)
//    .
//    .
//    .
//    b(FilterOrder+1)
//    a(1)
//    .
//    .
//    .
//    a(FilterOrder+1)
//
// NOTE: Coefficients must be represted as floating point
// ----------------------------------------------------------------------------
int PruLoadIirFilterParams(const char *file, iir_param_t* param) {
  FILE* fp = fopen(file, "r");
  float v;

  if (fp != NULL) {
    // First element is filter order
    fscanf(fp, "%u\n", &param->N);

    // Numerator coefficients
    for (int i=0; i<param->N+1; i++) {
      fscanf(fp,"%f\n", &v);
      param->b[i] = fix16_from_float(v);
    }

    // Denominator coefficients
    for (int i=0; i<param->N+1; i++) {
      fscanf(fp,"%f\n", &v);
      param->a[i] = fix16_from_float(v);
    }
    fclose(fp);
    return 0;
  }
  return -1;
}

void PruSprintIirParams(const iir_param_t* param, char* buff) {
  char temp[1024];

  sprintf(buff, "\n#IIR Filter Parameters(N = %i):\n", param->N);
  for (int i=0; i<param->N+1; i++) {
    sprintf(temp, "#\tb[%i] = %8.8f\ta[%i] = %8.8f\n",
            i, fix16_to_float(param->b[i]),
            i, fix16_to_float(param->a[i]));
    strcat(buff, temp);
    temp[0] = '\0';
  }
  strcat(buff, "#");
}

// ----------------------------------------------------------------------------
// Functions: void PruLoadNlbFilterParams(char* file)
//
// This function loads filter coeff from file to memory.
//
// file format:
//    fs (must be less than samping frequency of main loop)
//    alpha
//    beta
// ----------------------------------------------------------------------------
int PruLoadNlbFilterParams(const char* file, nlb_param_t* param) {
  FILE* fp = fopen(file, "r");
  float v;

  if (fp != NULL) {
    // First element is frequency
    fscanf(fp, "%u\n", &param->fs);

    // alpha
    fscanf(fp,"%f\n", &v);
    param->alpha = fix16_from_float(v);

    // alpha
    fscanf(fp,"%f\n", &v);
    param->beta = fix16_from_float(v);

    fclose(fp);
    return 0;
  }
  return -1;
}

void PruSprintNlbParams(const nlb_param_t* param, char* buff) {
  sprintf(buff, "\n#NLB Filter Parameters:\n"
          "#\tFs = %i (Hz)\n"
          "#\talpha = %f\n"
          "#\tbeta = %f\n#",
          param->fs,
          fix16_to_float(param->alpha),
          fix16_to_float(param->beta));
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
          "%4.3f\t"     // interaction_force - fix16_t -> float
          "%4.3f\t"     // flexion_pressure - fix16_t -> float
          "%4.3f\t"     // extension_pressure - fix16_t -> float
          "%4.3f\t"     // flexion_pressure_d - fix16_t -> float
          "%4.3f\t"     // extension_pressure_d - fix16_t -> float
          "%i\t"        // flexion_cmd - int32_t
          "%i\t"        // extension_cmd - int32_t
          "\n",
          st->time_stamp,
          fix16_to_float(st->interaction_force),
          fix16_to_float(st->flexion_pressure),
          fix16_to_float(st->extension_pressure),
          fix16_to_float(st->flexion_pressure_d),
          fix16_to_float(st->extension_pressure_d),
          st->flexion_cmd,
          st->extension_cmd
          );
}

void PruSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "interaction_force\t"
          "flexion_pressure\t"
          "extension_pressure\t"
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
