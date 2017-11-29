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

#include <stdint.h>
#include <stdio.h>

#include "fix16.h"
#include "filter.h"

// ----------------------------------------------------------------------------
// Initialize filter buffers
void iirInit(volatile iir_buff_t* filter, iir_coeff_t coeff)
{
  for(int i=0; i<coeff.N+1; i++){
    filter->x[i] = 0;
    filter->y[i] = 0;
  }
}

// ----------------------------------------------------------------------------
// Fixed point Q16.16 (really Q15.16) iir filter
fix16_t iirFilt(volatile iir_buff_t* filter, iir_coeff_t coeff, fix16_t in)
{
  fix16_t bx, ay, out;

  // Shift samples back in time
  for(int i=coeff.N; i>0; i--){
    filter->x[i] = filter->x[i-1];
    filter->y[i] = filter->y[i-1];
  }

  // new Sample
  filter->x[0] = in;

  // difference eq.
  out = fix16_smul(coeff.b[0], filter->x[0]);
  for(int k=1; k<coeff.N+1; k++) {
    bx = fix16_smul(coeff.b[k], filter->x[k]);
    ay = fix16_smul(coeff.a[k], filter->y[k]);
    out = fix16_sadd(out, fix16_ssub(bx, ay));
  }
  filter->y[0] = out;
  return out;
}

