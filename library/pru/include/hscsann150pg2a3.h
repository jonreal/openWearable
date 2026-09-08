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
  // Decimation (sensor owns "how often am I sampled"): hit the bus every
  // `decimate` ticks, staggered by `phase`; skip ticks return the held value.
  uint32_t decimate;
  uint32_t phase;
  uint32_t cnt;
  volatile fix16_t last;    // last decoded value, held across skip ticks
  volatile uint8_t fresh;   // 1 if this tick did a real read (for fault judging)
} pressure_sensor_t;

// Public. `decimate`/`phase` set the round-robin schedule at construction (read
// every `decimate` ticks, staggered by `phase`); pass (1, 0) to read every tick.
pressure_sensor_t* PressureSensorInit(i2cmux_t* mux,
                                      uint8_t mux_channel,
                                      uint8_t i2c_address,
                                      uint32_t decimate, uint32_t phase);
void PressureSensorFree(pressure_sensor_t* sensor);
fix16_t PressureSensorSample(pressure_sensor_t* sensor);

#endif /* _HSCSANN150PG2A3_H_ */
