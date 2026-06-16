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

// The board-specific values used below — rp_pru0, rp_pru1, pru_fw_prefix — are
// defined per SoC in board.c (declared in pru.h).

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


int PruWriteFirmware(char* suffix) {
  int rtn;
  char buf[64];
  char path[256];

  // PRU0
  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf), "%s-pru0-%s-fw", pru_fw_prefix, suffix);
  snprintf(path, sizeof(path), "%s/firmware", rp_pru0);
  int fd = open(path, O_WRONLY);
  if (fd == -1) {
    printf("pru0 - failed to open firmware location");
    return -1;
  }
  rtn = write(fd, buf, strlen(buf));
  close(fd);
  if (rtn == -1) {
    printf("pru0 - failed writing firmware location");
    return -1;
  }

  // PRU1
  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf), "%s-pru1-%s-fw", pru_fw_prefix, suffix);
  snprintf(path, sizeof(path), "%s/firmware", rp_pru1);
  fd = open(path, O_WRONLY);
  if (fd == -1) {
    printf("pru1 - failed to open firmware location");
    return -1;
  }
  rtn = write(fd, buf, strlen(buf));
  close(fd);
  if (rtn == -1) {
    printf("pru1 - failed writing firmware location");
    return -1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int PruWaitRunning(const char* rp_path)
//
//  Polls a remoteproc's "state" until it reports "running" (bounded ~1 s).
//  The PRU boot after a "start" write is asynchronous; on a cold run the
//  firmware load takes tens of ms, during which the PRU's memInit() zeroes
//  pru_ctl and would clobber an enable bit the A-core set too early. Waiting for
//  "running" here closes that race so memInit() completes before PruEnable().
//
// Outputs:   0 once running, -1 on timeout/error
// ---------------------------------------------------------------------------
static int PruWaitRunning(const char* rp_path) {
  char state_path[256] = {0};
  snprintf(state_path, sizeof(state_path), "%s/state", rp_path);
  char buf[64];
  for (int i = 0; i < 100; i++) {        // 100 * 10 ms = ~1 s ceiling
    int fd = open(state_path, O_RDONLY);
    if (fd == -1)
      return -1;
    memset(buf, 0, sizeof(buf));
    int rtn = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (rtn > 0 && strcmp(buf, "running\n") == 0)
      return 0;
    usleep(10000);                       // 10 ms between polls
  }
  return -1;
}

// ---------------------------------------------------------------------------
// Function: int PruInit(void)
//
//  This function restarts and loads pru firmware (rproc).
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int PruInit(char* suffix){
  int rtn;
  char buf[64] = {0};
  char path[256];

  if (PruRestart() == -1) {
    printf("restart pru failed.\n");
    return -1;
  }

  if (PruWriteFirmware(suffix) == -1) {
    printf("firmware write failed\n");
    return -1;
  }

  // PRU0
  snprintf(path, sizeof(path), "%s/state", rp_pru0);
  int fd = open(path, O_RDWR);
  if (fd == -1) {
    printf("pru0 - failed to open remoteproc driver");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  rtn = write(fd, "start", 5);
  close(fd);
  if (rtn == -1) {
    printf("pru0 - failed to start");
    printf(buf);
    return -1;
  }

  // PRU1
  snprintf(path, sizeof(path), "%s/state", rp_pru1);
  fd = open(path, O_RDWR);
  if (fd == -1) {
    printf("pru1 - failed to open remoteproc driver");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  rtn = write(fd, "start", 5);
  close(fd);
  if (rtn == -1) {
    printf("pru1 - failed to start");
    printf(buf);
    return -1;
  }

  // Wait for both cores to actually reach "running" before returning, so the
  // PRU's memInit() (which zeroes pru_ctl) finishes before the A-core enables.
  if (PruWaitRunning(rp_pru0) == -1) {
    printf("pru0 - did not reach running state\n");
    return -1;
  }
  if (PruWaitRunning(rp_pru1) == -1) {
    printf("pru1 - did not reach running state\n");
    return -1;
  }
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
  int rtn;
  char buf[64];
  char path[256];

  // PRU0
  snprintf(path, sizeof(path), "%s/state", rp_pru0);
  int fd = open(path, O_RDWR);
  if (fd == -1) {
    printf("pru0 - failed to open remoteproc driver");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  rtn = read(fd, buf, sizeof(buf));
  if (rtn == -1) {
    printf("pru0 - failed to read state");
    printf(buf);
    return -1;
  }
  if (strcmp(buf,"offline\n") == 0) {
    close(fd);
  } else {
    rtn = write(fd, "stop", 4);
    close(fd);
    if (rtn == -1) {
      printf("pru0 - failed to stop");
      printf(buf);
      return -1;
    }
  }

  // PRU1
  snprintf(path, sizeof(path), "%s/state", rp_pru1);
  fd = open(path, O_RDWR);
  if (fd == -1) {
    printf("pru1 - failed to open remoteproc driver");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  rtn = read(fd, buf, sizeof(buf));
  if (rtn == -1) {
    printf("pru1 - failed to read state");
    printf(buf);
    return -1;
  }
  if (strcmp(buf,"offline\n") == 0) {
    close(fd);
  } else {
    rtn = write(fd, "stop", 4);
    close(fd);
    if (rtn == -1) {
      printf("pru1 - failed to stop");
      printf(buf);
      return -1;
    }
  }
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
    printf("0x%X\t %u\t %i\t %i\n",
            db[i], db[i], (int32_t)db[i], fix16_to_int(db[i]));
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
      // Scale signal by 100 to store as int16_t
      l->lut[i] = (int16_t)fix16_to_int(fix16_from_float(value * 100.0));
    }
    fclose(fp);
    return 0;
  }
  printf("File not found\n");
  return -1;
}
