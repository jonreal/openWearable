#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#include "fix16.h"
#include "mem_types.h"

/* Globals ------------------------------------------------------------------*/
extern int debug;

/* Local globals ------------------------------------------------------------*/
FILE* flog;

/* Prototypes ---------------------------------------------------------------*/
void printDebugBuffer(void);
void initDebugBuffer(void);

int pru_run(const int pruNum, const char *const path);
int pru_init(void);
int pru_cleanup(void);
int pru_mem_init(void);
int armToPru0Interrupt(void);
int armToPru1Interrupt(void);

void writeState(uint8_t buffIndx);
void clearFlowBitFeild(void);
void enablePru(int en);

int buffer0Full(void);
int buffer1Full(void);
void clearBuffer0FullFlag(void);
void clearBuffer1FullFlag(void);
void clearBufferFlags(void);

uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void setKp(float Kp);
void setKd(float Kd);
void setAnklePos0(float pos0);
float getKp(void);
float getKd(void);
float getAnklePos0(void);

void printParameters(FILE *fp);
void printFirCoeff(FILE *fp);
void printFFLookUpTable(FILE *fp);

int logFileInit(char *fileName);
void closeLogFile(void);
void saveParameters(char *file);
int loadParameters(char *file);
int loadLookUpTable(char *file);
int loadIirFilterCoeff(char *file);
void setTareEncoderBit(void);
void resetGaitPhase(void);
void testFF(void);
void stopTestFF(void);
void enableFF(int en);
int FFenabled(void);

#endif


