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

#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#include "fix16.h"
#include "mem_types.h"

#define LOGSIZE   (4096 * 4096)
#define TEMP_BUFF_SIZE  1024
#define WRITE_BUFF_SIZE 65536

// Stuct ---------------------------------------------------------------------

// Log File Struct
typedef struct{
  uint32_t fd;
  uint32_t location;
  char* addr;
  char writeBuffer[WRITE_BUFF_SIZE];
} log_t;

// Circular Buffer Struct
typedef struct{
  uint32_t start;
  uint32_t end;
  char tempBuffer[TEMP_BUFF_SIZE];
} circbuff_t;

// External Globals ----------------------------------------------------------
extern int debug;

// Local globals -------------------------------------------------------------
FILE* flog;
log_t dataLog;
circbuff_t cbuff;

// Prototypes ----------------------------------------------------------------


void printDebugBuffer(void);
void initDebugBuffer(void);

void circBuffInit(void);
void circBuffUpdate(void);

int pru_restart(void);
int pru_init(void);
int pru_cleanup(void);
int pru_mmap(void);
void sprintMemoryAllocation(char* buffer);

void printState(uint32_t si, FILE* fp);
void writeState(void);

void clearFlowBitField(void);
void enablePru(int en);

uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void printParameters(FILE* fp);
void sprintParameters(char* buffer);
void printFirCoeff(FILE* fp);
void sprintFirCoeff(char* buffer);

void printStateHeader(FILE *fp);
//void sprintStateHeader(char* buffer);

int logFileInit(char* fileName);
int closeLogFile(void);
void saveParameters(char* file);
int loadParameters(char* file);
//int loadLookUpTable(char* file);
int loadIirFilterCoeff(char* file);
int loadNlbFilterParam(char* file);


#endif
