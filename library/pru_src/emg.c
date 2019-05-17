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


#include "emg.h"
#include <stdlib.h>
#include "adcdriver.h"

emg_t* EmgInitSensor(uint8_t chan) {
  emg_t* sens = malloc(sizeof(emg_t));
  sens->adc_ch = chan;
  return sens;
}

void EmgCleanup(emg_t* sens) {
  free(sens);
}

fix16_t EmgSample(const emg_t* sens) {
  return AdcSampleChmV(sens->adc_ch);
}
