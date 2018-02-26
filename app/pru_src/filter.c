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
void IirInit(const iir_param_t* param, volatile iir_filt_t* filt) {
  for(int i=0; i<param->N+1; i++){
    filt->x[i] = 0;
    filt->y[i] = 0;
  }
}

// ----------------------------------------------------------------------------
// Fixed point Q16.16 (really Q15.16) iir filter
fix16_t IirFilt(fix16_t in, const iir_param_t* param,
                volatile iir_filt_t* filt) {
  fix16_t bx, ay, out;

  // Shift samples back in time
  for(int i=param->N; i>0; i--){
    filt->x[i] = filt->x[i-1];
    filt->y[i] = filt->y[i-1];
  }

  // new Sample
  filt->x[0] = in;

  // difference eq.
  out = fix16_smul(param->b[0], filt->x[0]);
  for(int k=1; k<param->N+1; k++) {
    bx = fix16_smul(param->b[k], filt->x[k]);
    ay = fix16_smul(param->a[k], filt->y[k]);
    out = fix16_sadd(out, fix16_ssub(bx, ay));
  }
  filt->y[0] = out;
  return out;
}
