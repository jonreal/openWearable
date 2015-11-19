#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#include "mem_types.h"

#define PRU_SENSOR    0
#define PRU_CONTROL   1


/* Globals ------------------------------------------------------------------*/
extern int debug;

/* Local Globals- */
FILE* fid;
uint32_t collectionFlag;

/* Prototypes ---------------------------------------------------------------*/
void printDebugBuffer(void);
void initDebugBuffer(void);
int pru_run(const int pruNum, const char *const path);
int pru_init(void);
int pru_cleanup(void);
int pru_mem_init(void);
int armToPru0Interrupt(void);
int armToPru1Interrupt(void);
//void writeParams(param_mem_t inputs);

//void writeLookupTables(lookUp_mem_t lu);
void writeState(uint8_t buffIndx);
void clearFlowBitFeild(void);
void enable(void);
void disable(void);
int isBuffer0Full(void);
int isBuffer1Full(void);
void resetBuffer0FullFlag(void);
void resetBuffer1FullFlag(void);
uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void setKp(float Kp);
void setKd(float Kd);
void setAnklePos0(float pos0);
uint16_t getKp(void);
uint16_t getKd(void);
int16_t getAnklePos0(void);
int logFileInit(char *fileName);
void saveParameters(char *f);
void loadParameters(char *f);
void printParameters(void);
void closeLogFile(void);

void clearBufferFlags(void);
void setTareEncoderBit(void);
#endif


