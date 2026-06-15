#ifndef _PRU_H_
#define _PRU_H_

#include "mem_types.h"

#define HZ_TO_TICKS(hz) ((uint32_t)((float)PRU_CLK/hz))

#define PRU_CLK           200000000
#define PRU_GLOBAL_BASE		0x4A300000
#define PRU0_DRAM		      (PRU_GLOBAL_BASE)
#define PRU1_DRAM		      (PRU_GLOBAL_BASE + 0x00002000)
#define PRU_SHARED_DRAM		(PRU_GLOBAL_BASE + 0x00010000)

// Board remoteproc nodes: friendly, stable symlinks under /dev/remoteproc
// (kernel CONFIG_REMOTEPROC_CDEV + BeagleBone udev) pointing at the two PRU
// cores. pru.c builds "<path>/firmware" and "<path>/state" from these, so only
// these two paths and the firmware prefix differ per SoC (vs am64x, which uses
// /dev/remoteproc/j7-pru0_{0,1} and the "j721e" prefix). Firmware images are
// named "<PRU_FW_PREFIX>-pru{0,1}-<app>-fw" under /lib/firmware.
#define RP_PRU0_PATH   "/dev/remoteproc/pruss-core0"
#define RP_PRU1_PATH   "/dev/remoteproc/pruss-core1"
#define PRU_FW_PREFIX  "am335x"

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
