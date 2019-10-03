/* Copyright 2019 Jonathan Realmuto

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

#include "bno055.h"
#include <stdlib.h>

imu_t* ImuInit(i2c_t* i2c, uint8_t i2c_address) {
  imu_t* imu = malloc(sizeof(imu_t));

  imu->addrs = i2c_address;
  imu->i2c = i2c;
  imu->euler.x = 0;
  imu->euler.y = 0;
  imu->euler.z = 0;

  // read chip id (0xA0)
  imu->sensorID = I2cRxByte(imu->i2c, imu->addrs, BNO055_CHIP_ID_ADDR);
  __delay_cycles(50000);

  // set operation mode
  I2cTxByte(imu->i2c, imu->addrs, BNO055_OPR_MODE_ADDR, BNO055_MODE_CONFIG);
  __delay_cycles(50000);

  // set normal power mode
  I2cTxByte(imu->i2c, imu->addrs, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
  __delay_cycles(50000);

  // use external crystal
  I2cTxByte(imu->i2c, imu->addrs, BNO055_SYS_TRIGGER_ADDR, (1 << 7));
  __delay_cycles(50000);
  __delay_cycles(50000);


  // set units
  I2cTxByte(imu->i2c, imu->addrs, BNO055_UNIT_SEL_ADDR,
              (1 << 7) | // android orientation
              (0 << 5) | // celsius
              (0 << 3) | // degs
              (0 << 2) | // degs/s
              (0 << 1) // m/s^2
            );
  __delay_cycles(50000);

  // get seft test result
  uint8_t st = I2cRxByte(imu->i2c, imu->addrs, BNO055_SELFTEST_RESULT_ADDR);
  __delay_cycles(50000);

  // get system status
  uint8_t status = I2cRxByte(imu->i2c, imu->addrs, BNO055_SYS_STAT_ADDR);
  __delay_cycles(50000);

  // get system status
  uint8_t error = I2cRxByte(imu->i2c, imu->addrs, BNO055_SYS_ERR_ADDR);
  __delay_cycles(50000);

  // set NDOF mode
  I2cTxByte(imu->i2c, imu->addrs, BNO055_OPR_MODE_ADDR, BNO055_MODE_NDOF);
  __delay_cycles(50000);

//  // set IMU mode
//  I2cTxByte(imu->addrs, BNO055_OPR_MODE_ADDR, BNO055_MODE_IMUPLUS);
//  __delay_cycles(50000);

  // get system status
  status = I2cRxByte(imu->i2c, imu->addrs, BNO055_SYS_STAT_ADDR);
  __delay_cycles(50000);

  // get system status
  error = I2cRxByte(imu->i2c, imu->addrs, BNO055_SYS_ERR_ADDR);
  __delay_cycles(100000);
  __delay_cycles(100000);

  return imu;
}

void ImuUpdate(imu_t* imu) {
  uint8_t buffer[6];

  I2cRxBurst(imu->i2c, imu->addrs, BNO055_EULER_H_LSB_ADDR, 6, buffer);

  // 1 deg = 16 LSB (0x100000 in fix16)
  imu->euler.x = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buffer[0]) |
                                (((int16_t)buffer[1]) << 8))),0x100000);
  imu->euler.y = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buffer[2]) |
                                (((int16_t)buffer[3]) << 8))),0x100000);
  imu->euler.z = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buffer[4]) |
                                (((int16_t)buffer[5]) << 8))),0x100000);
}

euler_t ImuGetEuler(imu_t* imu) {
  return imu->euler;
}

//void ImuGetAngVel(imu_t* imu, volatile imu_angvel_t* angvel) {
//
//  uint8_t buffer[6];
//
//  I2cRxBurst(imu->addrs, BNO055_GYRO_DATA_X_LSB_ADDR, 6, buffer);
//
//  // 1 deg/s = 16 LSB (0x100000 in fix16)
//  angvel->x = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[0]) |
//                                (((int16_t)buffer[1]) << 8))),0x100000);
//  angvel->y = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[2]) |
//                                (((int16_t)buffer[3]) << 8))),0x100000);
//  angvel->z = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[4]) |
//                                (((int16_t)buffer[5]) << 8))),0x100000);
//}
//
//void ImuGetLinAcc(imu_t* imu, volatile imu_linacc_t* linacc) {
//
//  uint8_t buffer[6];
//
//  I2cRxBurst(imu->addrs, BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, 6, buffer);
//
//  // 1 m/s^2 = 100 LSB (0x640000 in fix16)
//  linacc->x = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[0]) |
//                                (((int16_t)buffer[1]) << 8))),0x640000);
//  linacc->y = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[2]) |
//                                (((int16_t)buffer[3]) << 8))),0x640000);
//  linacc->z = fix16_sdiv(
//                  fix16_from_int((int)(((int16_t)buffer[4]) |
//                                (((int16_t)buffer[5]) << 8))),0x640000);
//}
