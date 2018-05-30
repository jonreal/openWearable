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

#ifndef _FILTER_H_
#define _FILTER_H_

#include "fix16.h"

typedef struct{
  uint32_t N;
  fix16_t* b;
  fix16_t* a;
  volatile fix16_t* x;
  volatile fix16_t* y;
} iir_filt_t;

// --- Global Debug Buffer
extern volatile uint32_t* debug_buff;

// --- Prototypes
iir_filt_t* IirInit(const uint32_t filtorder,
                    const fix16_t* b_coeff, const fix16_t* a_coeff);
fix16_t IirFilt(fix16_t in, iir_filt_t* filt);
#endif
