#include "bno055_arm.h"
#include <stdlib.h>
#include <stdio.h>
#include "rc_i2c.h"
#include <unistd.h>

// Must init i2c first

imu_t* ImuInit(uint8_t address) {
  uint8_t rx = 0;
  imu_t* imu = malloc(sizeof(imu_t));
  imu->addr = address;
  imu->status = 0;
  imu->euler.x = 0;
  imu->euler.y = 0;
  imu->euler.z = 0;

  rc_i2c_set_device_address(1,imu->addr);

  // read chip id
  rc_i2c_read_byte(1, BNO055_CHIP_ID_ADDR, &rx);
//  printf("0x%X\n",rx);
  usleep(10000);

  // set operation mode
  rc_i2c_write_byte(1, BNO055_OPR_MODE_ADDR, BNO055_MODE_CONFIG);
  usleep(10000);

  // set operation mode
  rc_i2c_write_byte(1, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
  usleep(10000);

  // use external crystal
  rc_i2c_write_byte(1, BNO055_SYS_TRIGGER_ADDR, (1 << 7));
  usleep(50000);

  // set units
  rc_i2c_write_byte(1, BNO055_UNIT_SEL_ADDR,
              (1 << 7) | // android orientation
              (0 << 5) | // celsius
              (0 << 3) | // degs
              (0 << 2) | // degs/s
              (0 << 1) // m/s^2
            );
  usleep(50000);

  // system status
  rc_i2c_read_byte(1, BNO055_SYS_STAT_ADDR, &rx);
//  printf("0x%X\n",rx);
  usleep(10000);

  // set NDOF mode
  rc_i2c_write_byte(1, BNO055_OPR_MODE_ADDR, BNO055_MODE_NDOF);
  usleep(10000);

  // system status
  rc_i2c_read_byte(1, BNO055_SYS_STAT_ADDR, &rx);
//  printf("0x%X\n",rx);
  usleep(10000);

  printf("IMU initialized\n");
  return imu;
}


void ImuUpdate(imu_t* imu) {
  uint8_t buf[6];
  uint8_t status;
  rc_i2c_set_device_address(1,imu->addr);
  usleep(350);
  rc_i2c_read_byte(1, BNO055_CALIB_STAT_ADDR, &status);
  usleep(350);
  rc_i2c_read_bytes(1, BNO055_EULER_H_LSB_ADDR, 6, buf);

  // 1 deg = 16 LSB (0x100000 in fix16)
  imu->status = status;
  imu->euler.x = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buf[0]) |
                                (((int16_t)buf[1]) << 8))),0x100000);
  imu->euler.y = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buf[2]) |
                                (((int16_t)buf[3]) << 8))),0x100000);
  imu->euler.z = fix16_sdiv(
                  fix16_from_int((int)(((int16_t)buf[4]) |
                                (((int16_t)buf[5]) << 8))),0x100000);
}

euler_t ImuGetEuler(imu_t* imu) {
  return imu->euler;
}
