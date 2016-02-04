#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#include "mem_types.h"

#define PRU_SENSOR    0
#define PRU_CONTROL   1


/* Globals ------------------------------------------------------------------*/
extern int debug;

/* Local globals ------------------------------------------------------------*/
FILE* fid;

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
void enable(void);
void disable(void);

int isBuffer0Full(void);
int isBuffer1Full(void);
void clearBuffer0FullFlag(void);
void clearBuffer1FullFlag(void);
void clearBufferFlags(void);

uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void setKp(float Kp);
void setKd(float Kd);
void setAnklePos0(float pos0);
uint16_t getKp(void);
uint16_t getKd(void);
int16_t getAnklePos0(void);

int logFileInit(char *fileName);
void closeLogFile(void);
void saveParameters(char *f);
int loadParameters(char *f);
void printParameters(void);
int loadLookUpTable(char *file);
int loadIirFilterCoeff(char *file);
void printFFLookUpTable(void);
void printFirCoeff(void);
void setTareEncoderBit(void);
void toggleFeedforward(void);
void resetGaitPhase(void);
int getFFState(void);
void testFF(void);
void stopTestFF(void);

#endif


