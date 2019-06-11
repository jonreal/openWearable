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

#ifndef _HSCSANN150PG2A3_H_
#define _HSCSANN150PG2A3_H_

#include <stdint.h>
#include "fix16.h"
#include "pca9548a.h"

typedef struct {
  i2cmux_t* mux;
  uint8_t mux_channel;
  uint8_t i2c_address;
} pressure_sensor_t;

// Public
pressure_sensor_t* PressureSensorInit(i2cmux_t* mux,
                                      uint8_t mux_channel,
                                      uint8_t i2c_address);
void PressureSensorFree(pressure_sensor_t* sensor);
fix16_t PressureSensorSample(const pressure_sensor_t* sensor);

#endif /* _HSCSANN150PG2A3_H_ */
