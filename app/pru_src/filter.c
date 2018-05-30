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

#include "filter.h"
#include <stdlib.h>
#include "fix16.h"

// ----------------------------------------------------------------------------
// Initialize filter buffers
iir_filt_t* IirInit(const uint32_t filtorder,
                    const fix16_t* b_coeff, const fix16_t* a_coeff) {

  iir_filt_t* filt = malloc(sizeof(iir_filt_t));
  filt->N = filtorder;
  filt->b = malloc(sizeof(fix16_t)*(filtorder+1));
  filt->a = malloc(sizeof(fix16_t)*(filtorder+1));
  filt->x = malloc(sizeof(fix16_t)*(filtorder+1));
  filt->y = malloc(sizeof(fix16_t)*(filtorder+1));
  for (int i=0; i<filtorder+1; i++) {
    filt->x[i] = 0;
    filt->y[i] = 0;
    filt->b[i] = b_coeff[i];
    filt->a[i] = a_coeff[i];
  }
  return filt;
}

// ----------------------------------------------------------------------------
// Fixed point Q16.16 iir filter
fix16_t IirFilt(fix16_t in, iir_filt_t* filt) {
  fix16_t bx, ay, out;

  // Shift samples back in time
  for(int i=filt->N; i>0; i--){
    filt->x[i] = filt->x[i-1];
    filt->y[i] = filt->y[i-1];
  }

  // new Sample
  filt->x[0] = in;

  // difference eq.
  out = fix16_smul(filt->b[0], filt->x[0]);
  for(int k=1; k<filt->N+1; k++) {
    bx = fix16_smul(filt->b[k], filt->x[k]);
    ay = fix16_smul(filt->a[k], filt->y[k]);
    out = fix16_sadd(out, fix16_ssub(bx, ay));
  }
  filt->y[0] = out;
  return out;
}
