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

#include "halleffect.h"
#include <stdlib.h>
#include "adcdriver.h"

hall_t* HallInitSensor(uint8_t chan, fix16_t angle_offset) {
  hall_t* hall = malloc(sizeof(hall_t));
  hall->adc_chan = chan;
  hall->angle_offset_deg = angle_offset;
  return hall;
}

fix16_t HallGetAngleDeg(hall_t* hall) {
  fix16_t volts = adcSampleChmV(hall->adc_chan);
  return (fix16_ssub(fix16_smul(volts,FIX16_mVOLTS2DEG),
                     hall->angle_offset_deg));
}

fix16_t HallGetAngleRad(hall_t* hall) {
  return fix16_smul(HallGetAngleDeg(hall),FIX16_DEG2RAD);
}
