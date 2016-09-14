#ifndef _LSM6DS3IMU_H_
#define _LSM6DS3IMU_H_

// IMU Registers
#define LSM_WHO_AM_I    0x0F

// SPI channel
#define SPI_IMU_CHAN    0x0

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debugBuffer;

// Prototypes -----------------------------------------------------------------
void lsm_imuInit(void);
void lsm_x(void);

#endif
