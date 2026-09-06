/* Copyright 2026 Jonathan Realmuto

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

#include "potentiometer.h"
#include <stdint.h>
#include <stdlib.h>
#include "adcdriver.h"
#include "filter.h"
#include "fix16.h"

potentiometer_t* PotentiometerInit(uint8_t adc_ch, iir_filt_t* filter) {
  potentiometer_t* pot = malloc(sizeof(potentiometer_t));

  pot->adc_ch    = adc_ch;
  pot->filt      = filter;
  pot->value_raw = 0;
  pot->value     = 0;

  return pot;
}

void PotentiometerUpdate(potentiometer_t* pot) {
  pot->value_raw = AdcSampleChBits(pot->adc_ch);

  // Same pattern as PamUpdate: filter the raw sample if a filter is attached.
  if (pot->filt)
    pot->value = (uint32_t) fix16_to_int(
                   FiltIir(fix16_from_int((int) pot->value_raw), pot->filt));
  else
    pot->value = pot->value_raw;
}

void PotentiometerFree(potentiometer_t* pot) {
  free(pot);
}
