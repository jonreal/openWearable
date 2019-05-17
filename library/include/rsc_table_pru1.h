#ifndef _RSC_TABLE_AM335X_PRU_H_
#define _RSC_TABLE_AM335X_PRU_H_

#include <stddef.h>
#include <rsc_types.h>
#include "pru.h"
#include "pru_virtio_ids.h"


/* PRU Global Offsets */
#define PRU_INTC		(PRU_GLOBAL_BASE + 0x00020000)
#define PRU0_DEBUG		(PRU_GLOBAL_BASE + 0x00022400)
#define PRU1_DEBUG		(PRU_GLOBAL_BASE + 0x00024400)
#define PRU_CFG			(PRU_GLOBAL_BASE + 0x00026000)
#define PRU_UART0		(PRU_GLOBAL_BASE + 0x00028000)
#define PRU_IEP			(PRU_GLOBAL_BASE + 0x0002E000)
#define PRU_ECAP0		(PRU_GLOBAL_BASE + 0x00030000)
#define PRU0_IRAM		(PRU_GLOBAL_BASE + 0x00034000)
#define PRU1_IRAM		(PRU_GLOBAL_BASE + 0x00038000)

/* PRU Local Offsets */
//#define PRU_IRAM		(0x00000000)
//#define PRU_DRAM		(0x00000000)
//#define PRU_L_SHARED_DRAM	(0x00002000)
#define PRU_DRAM            (0x00000000)
#define PRU_OTHER_DRAM      (0x00002000)
#define PRU_L_SHARED_DRAM   (0x00010000)


/* Sizes */
#define PRU0_IRAM_SIZE		(SZ_8K)
#define PRU1_IRAM_SIZE		(SZ_8K)

#define PRU0_DRAM_SIZE		(SZ_8K)
#define PRU1_DRAM_SIZE		(SZ_8K)

#define PRU_SHARED_DRAM_SIZE	(SZ_8K + SZ_4K)

/*
 * Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define PRU_RPMSG_VQ0_SIZE	2
#define PRU_RPMSG_VQ1_SIZE	2

/* flip up bits whose indices represent features we support */
#define RPMSG_PRU_C0_FEATURES	1

/* Definition for unused interrupts */
#define HOST_UNUSED		255

struct my_resource_table {
	struct resource_table base;
	uint32_t offset[2]; /* Should match 'num' in actual definition */

};

#pragma DATA_SECTION(am335x_pru_remoteproc_ResourceTable, ".resource_table")
#pragma RETAIN(am335x_pru_remoteproc_ResourceTable)
struct my_resource_table am335x_pru_remoteproc_ResourceTable = {
	1,	/* we're the first version that implements this */
	0,	/* number of entries in the table */
	0, 0,	/* reserved, must be zero */
  0,
};

#endif /* _RSC_TABLE_AM335X_PRU_H_ */
