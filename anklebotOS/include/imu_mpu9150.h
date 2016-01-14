#ifndef _IMU_MPU9150_H_
#define _IMU_MPU9150_H_

/* ---- I2C Address ---- */
#define IMU_ADDR    0x68

/* ---- Registers (incomplete) ---- */
#define PWR_MGMT_1          0x6B
#define SMPRT_DIV           0x19
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define MEASURE_BASE_ADDR   0x3B

extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void imuInit(void);
void imuCleanUp(void);
void imuSample(volatile int16_t imu[6]);

#endif
