/* Copyright 2018-2019 Jonathan Realmuto

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

#include "sfm4100.h"
#include <stdlib.h>
#include "fix16.h"
#include "i2cdriver.h"

const uint8_t read_cmd = 0xF1;
const uint8_t soft_reset_cmd = 0xFE;

massflow_sensor_t* MassflowSensorInit(
  i2c_t* i2c, uint8_t address, uint8_t decimation) {
  massflow_sensor_t* self = malloc(sizeof(massflow_sensor_t));
  self->i2c = i2c;
  self->address = address;
  self->decimation = decimation;
  self->counter = -1; // start count at -1 to signal first sample
  self->value = 0;
  self->bits = 0;
  I2cTxByteNoReg(self->i2c, self->address, soft_reset_cmd);
  __delay_cycles(1000);

  return self;
}

void MassflowSensorFree(massflow_sensor_t* self) {
  free(self);
}

fix16_t MassflowSensorSample(massflow_sensor_t* self) {
  const static fix16_t bits2lpm = 0x42;    // 1/1000
  uint8_t buffer[2];


  // No decimation code
  //I2cTxByteNoReg(self->i2c, self->address, read_cmd);
  //__delay_cycles(1000);
  //I2cRxBurstNoReg(self->i2c, self->address, 2, buffer);
  //self->bits = (buffer[0] << 8) | buffer[1];
  //self->value = fix16_smul(fix16_from_int(self->bits), bits2lpm);

  // if first sample, issue read
  if (self->counter == -1) {
    self->counter = 0;
    I2cTxByteNoReg(self->i2c, self->address, read_cmd);
    __delay_cycles(1000);
  }
  self->counter++;
  if (self->counter >= self->decimation) {
    // Read ready data
    I2cRxBurstNoReg(self->i2c, self->address, 2, buffer);
    self->bits = (buffer[0] << 8) | buffer[1];
    self->value = fix16_smul(fix16_from_int(self->bits), bits2lpm);
    __delay_cycles(1000);
    // Issue next read command
    I2cTxByteNoReg(self->i2c, self->address, read_cmd);
    self->counter = 0;
  }
  return self->value;
}

