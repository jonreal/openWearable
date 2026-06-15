#ifndef _PRU_H_
#define _PRU_H_

#include "mem_types.h"

#define HZ_TO_TICKS(hz) ((uint32_t)((float)PRU_CLK/hz))

#define PRU_CLK           200000000
#define PRU_GLOBAL_BASE		0x4A300000
#define PRU0_DRAM		      (PRU_GLOBAL_BASE)
#define PRU1_DRAM		      (PRU_GLOBAL_BASE + 0x00002000)
#define PRU_SHARED_DRAM		(PRU_GLOBAL_BASE + 0x00010000)

// Board-specific values, defined per SoC in board.c: the two PRU remoteproc
// node paths and the firmware-image prefix. pru.c builds "<path>/firmware" and
// "<path>/state" from the node paths and names firmware
// "<pru_fw_prefix>-pru{0,1}-<app>-fw". These are the only board differences vs
// the am64x backend (which points rp_pru0/1 at /dev/remoteproc/j7-pru0_{0,1}
// with prefix "j721e").
extern const char* const rp_pru0;
extern const char* const rp_pru1;
extern const char* const pru_fw_prefix;

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

extern int PruLoadParams(const char* file, param_mem_t* param);
//extern int PruSaveParams(const char* file, param_mem_t* param);


#endif /* _PRU_H_ */
