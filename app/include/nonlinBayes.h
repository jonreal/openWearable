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

#ifndef _NONLINBAYES_H_
#define _NONLINBAYES_H_

#include "fix16.h"

#define N_BAYESBINS (32)

// --- Global Debug Buffer
extern volatile uint32_t *debugBuffer;

// --- Structs
typedef struct{
  uint32_t fs;
  fix16_t alpha;
  fix16_t beta;
} nlb_param_t;

typedef struct{
  volatile fix16_t prior[N_BAYESBINS];
  volatile fix16_t maxPosterior;
} nlb_buff_t;

// --- Prototypes
void nlbInit(volatile nlb_buff_t* filter, nlb_param_t param);
fix16_t nlbFilt(volatile nlb_buff_t* filter, nlb_param_t param, fix16_t in);

#endif
