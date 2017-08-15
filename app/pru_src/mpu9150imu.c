/* Copyright 2017 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include <stdint.h>

#include "i2cdriver.h"
#include "mpu9150imu.h"

int8_t imuInit(void)
{
  i2cInit();
  __delay_cycles(10000);

  // PWR_MGMT_1 (0x6B) :  SLEEP = 0x0 (no sleep)
  //                      TEMP_DIS = 0x1 (disable temp sensor)
  //                     CLKSEL = 0x1 (x-gyro clk)
  //                      ------------
  //                      = 0x09
  i2cTxByte(IMU_ADDR, PWR_MGMT_1, 0x9);
  __delay_cycles(10000);

  // SMPRT_DIV (0x19) : SMPLRT_DIV = 0x7 (1000 Hz)
  i2cTxByte(IMU_ADDR, SMPRT_DIV, 0x7);
  __delay_cycles(10000);

  // GYRO_CONFIG : FS_SEL = 0x0
  i2cTxByte(IMU_ADDR, GYRO_CONFIG, 0x0);
  __delay_cycles(10000);

  // ACCEL_CONFIG : AFS_SEL = 0x0
  i2cTxByte(IMU_ADDR, ACCEL_CONFIG, 0x0);
  __delay_cycles(10000);

  return 0;
}

void imuCleanUp(void)
{
  i2cCleanUp();
}

void imuSample(volatile int16_t imu[6])
{
  uint8_t buffer[14] = {0};

  i2cRxBurst(IMU_ADDR, MEASURE_BASE_ADDR, 14, buffer);

  // Accelerometers
  imu[0] = (buffer[0] << 8) | buffer[1];
  imu[1] = (buffer[2] << 8) | buffer[3];
  imu[2] = (buffer[4] << 8) | buffer[5];

  // Gyroscopes
  imu[3] = (buffer[8] << 8) | buffer[9];
  imu[4] = (buffer[10] << 8) | buffer[11];
  imu[5] = (buffer[12] << 8) | buffer[13];

}
