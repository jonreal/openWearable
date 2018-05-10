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

#include "interaction.h"
#include <stdlib.h>
#include "adcdriver.h"

#define FORCEBIAS  0x8000000

interact_t* InteractionInit(uint8_t chan){
  interact_t* sens = malloc(sizeof(interact_t));
  sens->adc_ch = chan;
  return sens;
}

void InteractionSampleForce(const interact_t* sens,
                            volatile fix16_t* interforce){
  *interforce =
    fix16_sdiv(fix16_from_int((int16_t)adcSampleCh(sens->adc_ch) - 2048),
               FORCEBIAS);
}

void InteractionFree(interact_t* sens) {
  free(sens);
}
