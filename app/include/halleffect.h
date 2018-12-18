/* Copyright 2018 Jonathan Realmuto

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

#ifndef _HALL_EFFECT_H_
#define _HALL_EFFECT_H_

#include <stdint.h>
#include "fix16.h"

// Driver for non-contacting hall effect single turn position sensor
// 6127 V1 A360 L.5 XX


// bits 2 deg
// currently using voltage divider to bring down output from 5.
// R1 = 4.7kOhm
// R2 = 2.2kOhm
// Vmax = R2/(R1 + R2) 5V
//
//
#define FIX16_mVOLTS2DEG 0x39CF

// Rad2deg
#define FIX16_DEG2RAD 0x478


typedef struct {
  fix16_t angle_offset_deg;
  uint8_t adc_chan;
} hall_t;


hall_t* HallInitSensor(uint8_t chan, fix16_t angle_offset);
fix16_t HallGetAngleDeg(hall_t* hall);
fix16_t HallGetAngleRad(hall_t* hall);



#endif
