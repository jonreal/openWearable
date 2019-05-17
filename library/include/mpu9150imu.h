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

extern volatile uint32_t *debug_buff;

/* ---- Prototypes ---- */
int8_t imuInit(void);
void imuCleanUp(void);
void imuSample(volatile int16_t imu[6]);

#endif
