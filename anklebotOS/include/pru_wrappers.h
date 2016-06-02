#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#include "fix16.h"
#include "mem_types.h"

#define LOGSIZE     (1e9 + 1)

// Stuct ---------------------------------------------------------------------

// Log File Struct
typedef struct{
  uint32_t fd;
  uint32_t location;
  char* addr;
  char writeBuffer[65536];
} log_t;

// Circular Buffer Struct
typedef struct{
  uint32_t start;
  uint32_t end;
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

int pru_run(const int pruNum, const char *const path);
int pru_init(void);
int pru_cleanup(void);
int pru_mem_init(void);
void printMemoryAllocation(FILE *fp);
void sprintMemoryAllocation(char* buffer);

int armToPru0Interrupt(void);
int armToPru1Interrupt(void);

void zeroState(uint32_t si);
void printState(uint32_t si, FILE* fp);
void writeState(void);

void clearFlowBitField(void);
void enablePru(int en);

uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void setKp(float Kp);
void setKd(float Kd);
void setAnklePos0(float pos0);
float getKp(void);
float getKd(void);
float getAnklePos0(void);

void printParameters(FILE* fp);
void sprintParameters(char* buffer);
void printFirCoeff(FILE* fp);
void sprintFirCoeff(char* buffer);
void printFFLookUpTable(FILE* fp);

void printStateHeader(FILE *fp);
void sprintStateHeader(char* buffer);

int logFileInit(char* fileName);
int closeLogFile(void);
void saveParameters(char* file);
int loadParameters(char* file);
int loadLookUpTable(char* file);
int loadIirFilterCoeff(char* file);

void setTareEncoderBit(void);

void startFFtest(void);
void stopFFtest(void);
void setFFenable(int en);
int getFFenable(void);
float getFFgain(void);
void setFFgain(float gain);

void setStepCurrent(float cur);
void startStepResponse(void);
void resetStepRespVars(void);

void resetGP(void);

#endif
