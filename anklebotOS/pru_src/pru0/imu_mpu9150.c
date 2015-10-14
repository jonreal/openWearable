#include <stdint.h>

#include "i2cdriver.h"
#include "imu_mpu9150.h"

void imuInit(void)
{
  i2cInit();
  __delay_cycles(10000);

  /* PWR_MGMT_1 (0x6B) :  SLEEP = 0x0 (no sleep)
   *                      TEMP_DIS = 0x1 (disable temp sensor)
   *                      CLKSEL = 0x1 (x-gyro clk)
   *                      ------------
   *                      = 0x09 */
  i2cTxByte(IMU_ADDR, PWR_MGMT_1, 0x9);
  __delay_cycles(10000);

  /* SMPRT_DIV (0x19) : SMPLRT_DIV = 0x7 (1000 Hz) */
  i2cTxByte(IMU_ADDR, SMPRT_DIV, 0x7);
  __delay_cycles(10000);

  /* GYRO_CONFIG : FS_SEL = 0x0 */
  i2cTxByte(IMU_ADDR, GYRO_CONFIG, 0x0);
  __delay_cycles(10000);

  /* ACCEL_CONFIG : AFS_SEL = 0x0 */
  i2cTxByte(IMU_ADDR, ACCEL_CONFIG, 0x0);
  __delay_cycles(10000);
}

void imuCleanUp(void)
{
  i2cCleanUp();
}

void imuSample(volatile int16_t imu[6])
{
  uint8_t buffer[14] = {0};

  i2cRxBurst(IMU_ADDR, MEASURE_BASE_ADDR, 14, buffer);

  /* Acc */
  imu[0] = (buffer[0] << 8) | buffer[1];
  imu[1] = (buffer[2] << 8) | buffer[3];
  imu[2] = (buffer[4] << 8) | buffer[5];

  /* Gyro */
  imu[3] = (buffer[8] << 8) | buffer[9];
  imu[4] = (buffer[10] << 8) | buffer[11];
  imu[5] = (buffer[12] << 8) | buffer[13];
}

void cleanUpImu(void)
{

}
