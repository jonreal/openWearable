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

#ifndef _PRESSURESENSOR_H_
#define _PRESSURESENSOR_H_

#include "fix16.h"

#define SENSOR_1 0x28

// max output in counts (2^14)
#define FIX16_RESOLUTION 0x4

// max pressure (150 psi -> 1054.20 pascal)
#define FIX16_PRESSURE_MAX 0x40A35C0

// scaling (150 psi / 2^14 bits)
#define BITS2PSI 0x258


// Global ---------------------------------------------------------------------
extern volatile uint32_t *debug_buff;

// Prototypes -----------------------------------------------------------------
void PressureSensorInit(void);
fix16_t PressureSensorSample(uint8_t sensorID);

#endif
