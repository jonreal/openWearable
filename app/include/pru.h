#ifndef _PRU_H_
#define _PRU_H_

// PRU Memory Map
#define PRU_GLOBAL_BASE		0x4A300000

// PRU Global Offsets
#define PRU0_DRAM		(PRU_GLOBAL_BASE)
#define PRU1_DRAM		(PRU_GLOBAL_BASE + 0x00002000)
#define PRU_SHARED_DRAM		(PRU_GLOBAL_BASE + 0x00010000)

// remote proc driver paths
#define PRU_UNBIND "/sys/bus/platform/drivers/pru-rproc/unbind"
#define PRU_BIND "/sys/bus/platform/drivers/pru-rproc/bind"
#define PRU0_NAME "4a334000.pru0"
#define PRU1_NAME "4a338000.pru1"
#define PRU_NAME_LEN 13
#define PRU0_UEVENT "/sys/bus/platform/drivers/pru-rproc/4a334000.pru0/uevent"
#define PRU1_UEVENT "/sys/bus/platform/drivers/pru-rproc/4a338000.pru1/uevent"

#endif /* _PRU_H_ */
