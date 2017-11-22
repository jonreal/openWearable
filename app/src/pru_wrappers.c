/* Copyright 2017 Jonathan Realmuto

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

#include "pru.h"
#include "mem_types.h"
#include "pru_wrappers.h"
#include "fix16.h"

#define AM33XX


// Global variables ----------------------------------------------------------

// Pointer to state data
shared_mem_t* s;

// Pointer to parameters data
param_mem_t* p;

// Pointer to lookup table data
lookUp_mem_t* l;

// Methods -------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function: void circBuffInit(void)
//
//  This function initializes the circular buffer.
// ---------------------------------------------------------------------------
void circBuffInit(void)
{
  cbuff.start = 0;
  cbuff.end = 0;
}

// ---------------------------------------------------------------------------
// Function: void circBuffInit(void)
//
//  This function initializes the circular buffer.
// ---------------------------------------------------------------------------
void circBuffUpdate(void)
{
  if (cbuff.end != s->stateIndex)
    cbuff.end = s->stateIndex;
}

// ---------------------------------------------------------------------------
// Function: void initDebugBuffer(void)
//
//  This function initializes the debug buffer. The debug buffer can be used to
//  info from PRUs to ARM.
// ---------------------------------------------------------------------------
void initDebugBuffer(void)
{
  for(int i=0; i<10; i++){
    p->debugBuffer[i] = 0;
  }
}

// ---------------------------------------------------------------------------
// Function: void printDebugBuffer(void)
//
//  This function prints the debug buffer.
// ---------------------------------------------------------------------------
void printDebugBuffer(void)
{
  printf("\n\n---- Debug Buffer ----\n");
  for(int i=0; i<10; i++){
    printf("0x%X \t%i\n", p->debugBuffer[i], p->debugBuffer[i]);
  }
}

// ---------------------------------------------------------------------------
// Function: int pru_init(void)
//
//  This function initializes restarts prus, then ensures prus are running.
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int pru_init(void)
{
  int bind_fd;

  if(pru_restart() == -1){
    printf("restart pru failed.\n");
    return -1;
  }
  // open file descriptors for pru rproc driver
  bind_fd = open(PRU_BIND, O_WRONLY);
  if(bind_fd == -1){
    printf("ERROR: pru-rproc driver missing\n");
    return -1;
  }
  // if pru0 is not loaded, load it
  if(access(PRU0_UEVENT, F_OK)!=0){
    if(write(bind_fd, PRU0_NAME, PRU_NAME_LEN)<0){
      printf("ERROR: pru0 bind failed\n");
      return -1;
    }
  }
  // if pru1 is not loaded, load it
  if(access(PRU1_UEVENT, F_OK)!=0){
    if(write(bind_fd, PRU1_NAME, PRU_NAME_LEN)<0){
      printf("ERROR: pru1 bind failed\n");
      return -1;
    }
  }
  close(bind_fd);
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int pru_restart(void)
//
//  This function restarts pru cores.
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int pru_restart(void)
{
  int unbind_fd, bind_fd;

  // open file descriptors for pru rproc driver
  unbind_fd = open(PRU_UNBIND, O_WRONLY);
  if(unbind_fd == -1){
    printf("open unbind fail\n");
    return -1;
  }
  bind_fd = open(PRU_BIND, O_WRONLY);
  if(bind_fd == -1){
    printf("open bind fail\n");
    return -1;
  }

  // if pru0 is loaded, unload it
  if(access(PRU0_UEVENT, F_OK)==0){
    if(write(unbind_fd, PRU0_NAME, PRU_NAME_LEN)<0){
      printf("ERROR: pru0 unbind failed\n");
      return -1;
    }
  }
  // if pru1 is loaded, unload it
  if(access(PRU1_UEVENT, F_OK)==0){
    if(write(unbind_fd, PRU1_NAME, PRU_NAME_LEN)<0){
      printf("ERROR: pru1 unbind failed\n");
      return -1;
    }
  }

  // now bind both
  if(write(bind_fd, PRU0_NAME, PRU_NAME_LEN)<0){
    printf("ERROR: pru0 bind failed\n");
    return -1;
  }
  if(write(bind_fd, PRU1_NAME, PRU_NAME_LEN)<0){
    printf("ERROR: pru1 bind failed\n");
    return -1;
  }
  close(unbind_fd);
  close(bind_fd);
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int pru_cleanup(const int pruNum)
//
//
// Outputs:   0 for success
// ---------------------------------------------------------------------------
int pru_cleanup(void)
{
  if(pru_restart() == -1){
    printf("restart pru failed.\n");
    return -1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// Function: int pru_mem_init(void)
//
//  This function sets memory pointers to pru0RAM, pru1RAM, sharedRAM
//
// Outputs:  returns 0 on success
// ---------------------------------------------------------------------------
int pru_mmap(void)
{
  void* ptr = NULL;
  int fd;
  char buf[1024] = {0,};

  fd = open("/dev/mem", O_RDWR | O_SYNC);

  // Memory Map for params (pru0 DRAM)
  ptr = mmap(0, sizeof(param_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU0_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  p = (param_mem_t *) ptr;
  ptr = NULL;
  initDebugBuffer();

  // Memory Map for lookup table (pru1 DRAM)
  ptr = mmap(0, sizeof(lookUp_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU1_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  l = (lookUp_mem_t *) ptr;
  ptr = NULL;

  // Memory Map for state (shared DRAM)
  ptr = mmap(0, sizeof(shared_mem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_SHARED_DRAM);
  if (ptr == MAP_FAILED) {
    printf ("ERROR: could not map memory.\n\n");
    return 1;
  }
  s = (shared_mem_t *) ptr;

  // Zero State
  for(int i=0; i<SIZE_STATE_BUFF; i++)
    memset(&(s->state[i]), 0, sizeof(state_t));

  clearFlowBitField();
  sprintMemoryAllocation(buf);
  fprintf(stdout,buf);
  close(fd);

  return 0;
}

// ---------------------------------------------------------------------------
// Function: void sprintMemoryAllocation(char* buffer)
//
//  This function prints the size of each chunk of memory, eg, shared memory,
//  parameter memory and lookupt table. The second function stores the string
//  in a buffer.
//
//  Input: pointer to file or buffer
// ---------------------------------------------------------------------------
void sprintMemoryAllocation(char* buffer)
{
  sprintf(buffer,
          "#\n#Memory Allocation:\n"
          "#\tParameter memory: %i bytes. p -> %p\n"
          "#\tLookup table memory: %i bytes. l -> %p\n"
          "#\tData memory: %i bytes. s -> %p\n#",
          sizeof(*p), p, sizeof(*l), l, sizeof(*s), s);
}

// ---------------------------------------------------------------------------
// Function: void printStateHeader(FILE *fp)
//           void sprintStateHeader(char* buffer)
//
//  This function prints the state header.
//
// Input: pointer to file or buffer
// ---------------------------------------------------------------------------
void printStateHeader(FILE *fp)
{
  fprintf(fp,
          "\n# frame\t"
          "emg_raw"
          "\n");
  fflush(fp);
}

void sprintStateHeader(char* buffer)
{
  sprintf(buffer,
          "\n# frame\t"
          "emg_raw"
          "\n");
}

// ---------------------------------------------------------------------------
// Function: void printState(uint8_t si, FILE *fp)
//           void sprintState(uint8_t si, char* buffer)
//
//  This function prints the states.
//
// Input: pointer to file or buffer
// ---------------------------------------------------------------------------
void printState(uint32_t si, FILE *fp)
{
  fflush(fp);
  fprintf(fp,
          "%u\t"    // timeStamp - uint32_t
          "%4.3f"    // emg_raw - fix16_t -> float
          "\n", s->state[si].timeStamp,
                fix16_to_float(s->state[si].emg_raw)
               );
  fflush(fp);
}

void sprintState(uint8_t si, char* buffer)
{
  sprintf(buffer,
          "%u\t"    // timeStamp - uint32_t
          "%4.3f"    // emg_raw - fix16_t -> float
          "\n", s->state[si].timeStamp,
                fix16_to_float(s->state[si].emg_raw)
               );
}

// ----------------------------------------------------------------------------
// Function: void writeState(void)
//
// This function writes state (data) to file. If debug mode is on, this
// prints low fidelity data to screen.
//
// Outputs: none
//
// Inputs: uint32_t bi - buffer index
// ------------------------------------------------------------------------- */
void writeState(void)
{
  int len = 0;
  int size = 0;

  memset(dataLog.writeBuffer, 0, WRITE_BUFF_SIZE);

  // Debug Mode, print first state if its new
  if(debug){
    if (cbuff.start != s->state[0].timeStamp){
      printState(0, stdout);
      cbuff.start = s->state[0].timeStamp;
    }
  }

  // Write Data Mode, use circular buffer
  else{
    size = ((SIZE_STATE_BUFF + cbuff.end - cbuff.start) % SIZE_STATE_BUFF);

    for(int i=cbuff.start; i<cbuff.start+size; i++){
      memset(cbuff.tempBuffer, 0, TEMP_BUFF_SIZE);
      sprintState(i % SIZE_STATE_BUFF, cbuff.tempBuffer);
      strcat(dataLog.writeBuffer, cbuff.tempBuffer);
    }

    cbuff.start = (cbuff.start + size) % SIZE_STATE_BUFF;

    // Write to file
    len = strlen(dataLog.writeBuffer);
    memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
    dataLog.location += len;
  }
}

// ---------------------------------------------------------------------------
// Function: void clearFlowBitFeild(void)
//
// This function clears flow bit feild used by prus.
// ---------------------------------------------------------------------------
void clearFlowBitField(void)
{
  s->cntrl = 0x00;
}

// ----------------------------------------------------------------------------
// Function: void enable(void)
//
//  This function sets the enbable bit in the flow bit feild to 1 (enable).
// ---------------------------------------------------------------------------
void enablePru(int en)
{
  if(en == 1)
    s->cntrl_bit.enable = 1;
  else
    s->cntrl_bit.enable = 0;
}

// ---------------------------------------------------------------------------
// Function: uint32_t hzToPruTicks(float freq_hz)
//
//  This function converts freq in Hz to number of pru clock ticks.
//
// Inputs:    freq_hz   -   freq. in hz (float)
//
// Outputs:   pruTicks  -   pru clock ticks (uint32_t)
// ---------------------------------------------------------------------------
uint32_t hzToPruTicks(float freq_hz)
{
  return (uint32_t)( (float)PRU_CLK / freq_hz);
}

// ---------------------------------------------------------------------------
// Function: float pruTicksToHz(uint32_t ticks)
//
//  This function converts pru ticks to freq in Hz.
//
// Inputs:    ticks     -   pru clock ticks (uint32_t)
//
// Outputs:   freq_hz   -   freq_hz (float)
// ---------------------------------------------------------------------------
float pruTicksToHz(uint32_t ticks)
{
  return 1/((float)ticks/200000000.0);
}

/* ----------------------------------------------------------------------------
 * Functions: int logFileInit(char* fileName)
 *
 * This function creates a log file.
 *
 * TODO: check if file exists.
 * ------------------------------------------------------------------------- */
int logFileInit(char* fileName)
{
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  int len = 0;
  char timeStr[1024];

  // Init datalog vars.
  dataLog.fd = 0;
  dataLog.location = 0;
  dataLog.addr = NULL;
  memset(&dataLog.writeBuffer[0], 0, sizeof(dataLog.writeBuffer));

  // Open file, stretch and write blank
  dataLog.fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC);
  if (lseek(dataLog.fd, LOGSIZE, SEEK_SET) == -1){
    close(dataLog.fd);
    printf("Error stretching file.\n");
    return -1;
  }
  if (write(dataLog.fd, "", 1) == -1){
    close(dataLog.fd);
    printf("Error writing blank at end of file.\n");
    return -1;
  }

  // memory map file
  dataLog.addr = mmap(0, LOGSIZE,
                         PROT_WRITE,
                         MAP_SHARED | MAP_POPULATE,
                         dataLog.fd, 0);
  if (dataLog.addr == MAP_FAILED){
    int er = errno;
    printf("mmap failed, %i\n",er);
    close(dataLog.fd);
    return -1;
  }

  mlock(dataLog.addr, LOGSIZE);

  // Log time
  memcpy(dataLog.writeBuffer, "#Date: ", 7);
  strftime(timeStr, 1024, "%d-%b-%Y %X\n", t);
  strcat(dataLog.writeBuffer, timeStr);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log memory allocation
  sprintMemoryAllocation(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log parameters
  sprintParameters(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log Fir Coeff
  sprintFirCoeff(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log header
  sprintStateHeader(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  return 0;
}

int closeLogFile(void)
{

  munlock(dataLog.addr, LOGSIZE);

  // Unmap and truncate file
  if (munmap(dataLog.addr, LOGSIZE) == -1){
    close(dataLog.fd);
    printf("unmap failed\n");
    return -1;
  }
  if (ftruncate(dataLog.fd, dataLog.location) == -1){
    close(dataLog.fd);
    printf("Error truncating file.\n");
    return -1;
  }
  close(dataLog.fd);

  return 0;
}



/* ----------------------------------------------------------------------------
 * Functions: void saveParameters(char* file)
 *
 * This function saves current parameters to file.
 * ------------------------------------------------------------------------- */
void saveParameters(char* file)
{
  FILE* fp = fopen(file, "w");
  if(fp != NULL){
    fprintf(fp, "%i\t// Freq. Ticks\n", 0);
    fclose(fp);
  }
  else{
    printf("\nParameter file doesn't exsist.\n");
  }
}

/* ----------------------------------------------------------------------------
 * Functions: void loadParameters(char* file)
 *
 * This function loads parameters from file.
 * ------------------------------------------------------------------------- */
int loadParameters(char* file)
{
  FILE* fp = fopen(file, "r");

  if(fp != NULL){
    fscanf(fp, "%u%*[^\n]\n", &p->frq_hz);

    fclose(fp);
    p->frq_clock_ticks = hzToPruTicks(p->frq_hz);

    return 0;
  }
  return -1;
}

/* ----------------------------------------------------------------------------
 * Functions: void printParameters(void)
 *
 * This function prints current parameters.
 * ------------------------------------------------------------------------- */
void printParameters(FILE *fp)
{
  fprintf(fp, "\n#Parameters:\n"
          "#\tFrq = %i (Hz)\n#",
          p->frq_hz );
  fflush(fp);
}

void sprintParameters(char* buffer)
{
  sprintf(buffer, "\n#Parameters:\n"
          "#\tFrq = %i (Hz)\n#",
          p->frq_hz);
}


/* ----------------------------------------------------------------------------
 * Functions: void loadLookUpTable(char* file)
 *
 * This function loads lookup table (Feedforward) from file to memory.
 * ------------------------------------------------------------------------- */
int loadLookUpTable(char* file)
{
  FILE* fp = fopen(file, "r");
  float value;

  if(fp != NULL){
    for(int i=0; i<NUM_FF_LT; i++){
      fscanf(fp, "%f\n", &value);

      // Scale signal by 1000 to store as int16_t
      l->u_ff[i] = (int16_t) fix16_to_int(fix16_from_float(value * 1000.0));
    }
    fclose(fp);
    return 0;
  }
  printf("File not found\n");
  return -1;
}

/* ----------------------------------------------------------------------------
 * Functions: void loadFilterCoeff(char* file)
 *
 * This function loads filter coeff from file to memory.
 *
 * file format:
 *    FilterOrder
 *    b(1)
 *    .
 *    .
 *    .
 *    b(FilterOrder+1)
 *    a(1)
 *    .
 *    .
 *    .
 *    a(FilterOrder+1)
 *
 * NOTE: Coefficients must be represted as floating point
 * ------------------------------------------------------------------------- */
int loadIirFilterCoeff(char *file)
{
  FILE* fp = fopen(file, "r");
  float v;

  if(fp != NULL){

    // First element is filter order
    fscanf(fp, "%u\n", &p->filt.N);

    // Numerator coefficients, b
    for(int i=0; i<p->filt.N+1; i++){
      fscanf(fp,"%f\n", &v);
      p->filt.b[i] = fix16_from_float(v);
    }

    // Denominator coefficients, a
    for(int i=0; i<p->filt.N+1; i++){
      fscanf(fp,"%f\n", &v);
      p->filt.a[i] = fix16_from_float(v);
    }
    fclose(fp);
    return 0;
  }
  return -1;
}
void printFirCoeff(FILE *fp)
{
  fprintf(fp, "\n#Filter Coefficients (N = %i):\n", p->filt.N);
  for(int i=0; i<p->filt.N+1; i++){
    fprintf(fp, "#\tb[%i] : %8.8f\ta[%i] : %8.8f\n",
            i, fix16_to_float(p->filt.b[i]),
            i, fix16_to_float(p->filt.a[i]));
  }
  fprintf(fp, "#");
  fflush(fp);
}

void sprintFirCoeff(char* buffer)
{
  char temp[1024];

  sprintf(buffer, "\n#Filter Coefficients (N = %i):\n", p->filt.N);
  for(int i=0; i<p->filt.N+1; i++){
    sprintf(temp, "#\tb[%i] : %8.8f\ta[%i] : %8.8f\n",
            i, fix16_to_float(p->filt.b[i]),
            i, fix16_to_float(p->filt.a[i]));
    strcat(buffer, temp);
    temp[0] = '\0';
  }
  strcat(buffer, "#");
}

void printFFLookUpTable(FILE *fp)
{
  for(int i=0; i<NUM_FF_LT; i++){
    fprintf(fp, "\t%i\t%i\n", i, l->u_ff[i]);
  }
}

