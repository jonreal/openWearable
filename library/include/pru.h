#ifndef _PRU_H_
#define _PRU_H_

#include "mem_types.h"

#define HZ_TO_TICKS(hz) ((uint32_t)((float)PRU_CLK/hz))

#define PRU_CLK           200000000
#define PRU_GLOBAL_BASE		0x4A300000
#define PRU0_DRAM		      (PRU_GLOBAL_BASE)
#define PRU1_DRAM		      (PRU_GLOBAL_BASE + 0x00002000)
#define PRU_SHARED_DRAM		(PRU_GLOBAL_BASE + 0x00010000)

// remote proc driver paths
#define PRU0_FW "/sys/class/remoteproc/remoteproc1/firmware"
#define PRU1_FW "/sys/class/remoteproc/remoteproc2/firmware"
#define PRU0_STATE "/sys/class/remoteproc/remoteproc1/state"
#define PRU1_STATE "/sys/class/remoteproc/remoteproc2/state"

int PruMemMap(pru_mem_t* pru_mem);
int PruInit(void);
int PruRestart(void);
int PruCleanup(void);
void PruCtlReset(pru_ctl_t* ctl);
void PruSprintMalloc(const pru_mem_t* pru_mem, char* buff);
int PruLoadParams(const char* file, param_mem_t* param);
void PruEnable(int en, pru_ctl_t* ctl);
void PruPrintDebugBuffer(const volatile uint32_t* db);
int PruLoadLut(char* file, lut_mem_t* l);

#endif /* _PRU_H_ */
