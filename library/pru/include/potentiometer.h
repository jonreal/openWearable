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

#ifndef _POT_H_
#define _POT_H_

#include <stdint.h>
#include "filter.h"              // iir_filt_t + FiltIir (same filter as pam_t)

typedef struct{
  uint8_t adc_ch;
  iir_filt_t* filt;              // optional IIR filter (NULL = none), like pam_t
  volatile uint32_t value_raw;   // raw ADC sample
  volatile uint32_t value;       // filtered (or raw if no filter)
} potentiometer_t;

potentiometer_t* PotentiometerInit(uint8_t adc_ch, iir_filt_t* filter);
void PotentiometerUpdate(potentiometer_t* pot);
void PotentiometerFree(potentiometer_t* pot);


#endif
