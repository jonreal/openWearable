/* Copyright 2025 Jonathan Realmuto

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

#ifndef _SFM4100_H_
#define _SFM4100_H_

#include <stdint.h>
#include "fix16.h"
#include "pca9548a.h"

typedef struct {
  i2c_t* i2c;
  uint8_t address;
  uint32_t decimation;
  int32_t counter;
  int32_t bits;
  fix16_t value;
} massflow_sensor_t;

// Public
massflow_sensor_t* MassflowSensorInit(
  i2c_t* i2c, uint8_t address, uint8_t decimation);
void MassflowSensorFree(massflow_sensor_t* self);
fix16_t MassflowSensorSample(massflow_sensor_t* self);

#endif /* _SFM4100_H_ */
