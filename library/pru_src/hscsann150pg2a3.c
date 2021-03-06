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

#include "hscsann150pg2a3.h"
#include <stdlib.h>
#include "i2cdriver.h"

pressure_sensor_t* PressureSensorInit(i2cmux_t* mux,
                                      uint8_t mux_channel,
                                      uint8_t i2c_address) {
  pressure_sensor_t* sensor = malloc(sizeof(pressure_sensor_t));
  if(mux) {
    sensor->mux = mux;
    sensor->mux_channel = mux_channel;
  } else {
    sensor->mux = NULL;
    sensor->mux_channel = NULL;
  }
  sensor->i2c_address = i2c_address;
  return sensor;
}

void PressureSensorFree(pressure_sensor_t* sensor) {
  free(sensor);
}


// Pressure = (bits - res * 0.1) * Pmax/ (res*0.8)
//          = (bits - 2^14 * 0.1) * Pmax/(2^14*0.8)
//          = (bits - Kp) * Mp;
//          K = 2^14*0.1, M = 1.05420 kPa / (0.8 * 2^14)
fix16_t PressureSensorSample(const pressure_sensor_t* sensor) {
  const static fix16_t Kp = 0x6666668;
  const static fix16_t Mp = 0x2EE;
  uint8_t buffer[4];

  if(sensor->mux) {
    MuxI2cSetChannel(sensor->mux, sensor->mux_channel);
    __delay_cycles(1100);
    I2cRxBurstNoReg(sensor->mux->i2c, sensor->i2c_address, 4, buffer);
    __delay_cycles(500);
  } else {
    I2cRxBurstNoReg(sensor->mux->i2c, sensor->i2c_address, 4, buffer);
  }

  uint32_t bits = (((uint32_t) (buffer[0] & ~0xC0)) << 8)
                    | ((uint32_t) buffer[1]);

  return fix16_smul(fix16_ssub(fix16_from_int(bits),Kp),Mp);
}
