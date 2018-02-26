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

#define MAX_IIR_ORDER 3

// Filter IIR buffers
typedef struct{
  volatile fix16_t x[MAX_IIR_ORDER+1];
  volatile fix16_t y[MAX_IIR_ORDER+1];
} iir_filt_t;

// IIR Coefficients
typedef struct{
  uint32_t N;
  fix16_t b[MAX_IIR_ORDER+1];
  fix16_t a[MAX_IIR_ORDER+1];
} iir_param_t;

// --- Global Debug Buffer
extern volatile uint32_t *debug_buff;

// --- Prototypes
void IirInit(const iir_param_t* param, volatile iir_filt_t* filt);
fix16_t IirFilt(fix16_t in, const iir_param_t* param,
                volatile iir_filt_t* filt);
#endif
