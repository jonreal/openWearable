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

#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#include <stdint.h>
#include "fix16.h"

typedef struct {
  uint8_t adc_ch;
} interact_t;

interact_t* InteractionInit(uint8_t chan);
void InteractionSampleForce(const interact_t* sens,
                            volatile fix16_t* interforce);
void InteractionFree(interact_t* sens);

#endif /* _INTERACTION_H_ */
