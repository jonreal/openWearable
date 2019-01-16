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

typedef struct{
  uint32_t N;
  fix16_t* b;
  volatile fix16_t* x;
} fir_filt_t;

typedef struct{
  uint32_t N;
  fix16_t a;
  fix16_t b;
  fix16_t c;
  fix16_t* x_n;
  fix16_t* inv_x_n;
  volatile fix16_t* prior;
  volatile fix16_t* posterior;
} nlb_filt_t;

typedef struct{
  uint32_t N;
  uint32_t dec;
  fix16_t kappa;
  fix16_t eta;
  fix16_t* x_n;
  fix16_t* inv_x_n;
  volatile fix16_t* prior;
  volatile fix16_t* posterior;
  volatile fix16_t m_sample;
  volatile fix16_t m;
  volatile uint32_t i0;
  volatile fix16_t prev_map;
  volatile uint32_t count;
  volatile uint32_t elements_per_cycle;


} nlbdec_filt_t;



// --- Global Debug Buffer
extern volatile uint32_t* debug_buff;

// --- Prototypes
iir_filt_t* FiltIirInit(const uint32_t filtorder,
                        const fix16_t* b_coeff,
                        const fix16_t* a_coeff);
fix16_t FiltIir(fix16_t in, iir_filt_t* f);

fir_filt_t* FiltFirInit(const uint32_t filtorder, const fix16_t* b_coeff);
fix16_t FiltFir(fix16_t in, fir_filt_t* f);


nlb_filt_t* FiltNlbInit(const uint32_t nbins,
                            const fix16_t fs,
                            const fix16_t max,
                            const fix16_t alpha,
                            const fix16_t beta);

fix16_t FiltNlb(fix16_t in, nlb_filt_t* f);

nlbdec_filt_t* FiltNlbDecInit(const uint32_t nbins,
                          const uint32_t dec,
                          const fix16_t fs,
                          const fix16_t max,
                          const fix16_t alpha,
                          const fix16_t beta);

//fix16_t FiltDecNlb(fix16_t in, nlbdec_filt_t* f);
#endif
