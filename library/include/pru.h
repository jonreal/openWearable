#ifndef _PRU_H_
#define _PRU_H_

#include "mem_types.h"

#define HZ_TO_TICKS(hz) ((uint32_t)((float)PRU_CLK/hz))
#define PRU_CLK           200000000

#define PRU_SHARED_RAM_BASE 0x30010000
#define PRU_SHARED_RAM_SIZE 0x00010000


int PruMemMap(pru_mem_t* pru_mem);
int PruInit(char* suffix);
int PruWriteFirmware(char* suffix);
int PruRestart(void);
int PruCleanup(void);
void PruCtlReset(pru_ctl_t* ctl);
void PruSprintMalloc(const pru_mem_t* pru_mem, char* buff);
void PruEnable(int en, pru_ctl_t* ctl);
void PruPrintDebugBuffer(const volatile uint32_t* db);
int PruLoadLut(char* file, lut_mem_t* l);
int PruFindPath(char* path, size_t len, const char* target);


extern int PruLoadParams(const char* file, param_mem_t* param);
//extern int PruSaveParams(const char* file, param_mem_t* param);


#endif /* _PRU_H_ */
