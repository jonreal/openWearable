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
iir_filt_t* FiltIirInit(const uint32_t filtorder,
                    const fix16_t* b_coeff, const fix16_t* a_coeff) {
  iir_filt_t* f = malloc(sizeof(iir_filt_t));
  f->N = filtorder;
  f->b = malloc(sizeof(fix16_t)*(filtorder+1));
  f->a = malloc(sizeof(fix16_t)*(filtorder+1));
  f->x = malloc(sizeof(fix16_t)*(filtorder+1));
  f->y = malloc(sizeof(fix16_t)*(filtorder+1));
  for (int i=0; i<filtorder+1; i++) {
    f->x[i] = 0;
    f->y[i] = 0;
    f->b[i] = b_coeff[i];
    f->a[i] = a_coeff[i];
  }
  return f;
}

// ----------------------------------------------------------------------------
fix16_t FiltIir(fix16_t in, iir_filt_t* f) {
  for(int i=f->N; i>0; i--){
    f->x[i] = f->x[i-1];
    f->y[i] = f->y[i-1];
  }

  f->x[0] = in;

  fix16_t out = fix16_smul(f->b[0], f->x[0]);
  for(int k=1; k<f->N+1; k++) {
    out = fix16_sadd(out,fix16_ssub(
                          fix16_smul(f->b[k], f->x[k]),
                          fix16_smul(f->a[k], f->y[k])));
  }
  f->y[0] = out;
  return out;
}

// ----------------------------------------------------------------------------
fir_filt_t* FiltFirInit(const uint32_t filtorder, const fix16_t* b_coeff) {
  fir_filt_t* f = malloc(sizeof(fir_filt_t));
  f->N = filtorder;
  f->b = malloc(sizeof(fix16_t)*(filtorder+1));
  f->x = malloc(sizeof(fix16_t)*(filtorder+1));
  for (int i=0; i<filtorder+1; i++) {
    f->x[i] = 0;
    f->b[i] = b_coeff[i];
  }
  return f;
}

// ----------------------------------------------------------------------------
fix16_t FiltFir(fix16_t in, fir_filt_t* f) {
  for(int i=f->N; i>0; i--){
    f->x[i] = f->x[i-1];
  }

  f->x[0] = in;

  fix16_t out = fix16_smul(f->b[0], f->x[0]);
  for(int k=1; k<f->N+1; k++) {
    out = fix16_sadd(out, fix16_smul(f->b[k], f->x[k]));
  }
  return out;
}

// ----------------------------------------------------------------------------
nlb_filt_t* FiltNlbInit(const uint32_t nbins,
                        const fix16_t fs,
                        const fix16_t max,
                        const fix16_t alpha,
                        const fix16_t beta){

  nlb_filt_t* f = malloc(sizeof(nlb_filt_t));
  f->x_n = malloc(sizeof(fix16_t)*nbins);
  f->inv_x_n = malloc(sizeof(fix16_t)*nbins);
  f->prior = malloc(sizeof(fix16_t)*nbins);
  f->posterior = malloc(sizeof(fix16_t)*nbins);
  f->N = nbins;

  fix16_t kappa = fix16_sdiv(fix16_smul(fix16_smul(
                  fix16_from_int(nbins),fix16_from_int(nbins)),alpha),fs);
  fix16_t eta = fix16_sdiv(beta,fs);

  // Minimum parameters
  if (fix16_ssub(eta,0x1) < 0) {
    eta = (fix16_t)0x1;
  }
  if (fix16_ssub(kappa,0x1) < 0) {
    kappa = (fix16_t)0x1;
  }

  f->a = fix16_ssub(fix16_ssub(fix16_ssub(fix16_one,kappa),kappa),eta);
  f->b = kappa;
  f->c = fix16_smul(fix16_from_int(1000),eta);

  for (int i=0; i<nbins; i++) {
    f->prior[i] = fix16_sdiv(fix16_from_int(1000),fix16_from_int(nbins));
    f->posterior[i] = (fix16_t)0;
    f->x_n[i] = fix16_smul(fix16_sdiv(max,fix16_from_int(nbins)),
                              fix16_from_int(i+1));
    f->inv_x_n[i] = fix16_sdiv(fix16_one,f->x_n[i]);
  }
  return f;
}

// ----------------------------------------------------------------------------
fix16_t FiltNlb(fix16_t in, nlb_filt_t* f){
  fix16_t pp, y_hat, sum;
  static const fix16_t f16_1000 = 0x3E80000;

  // First end point
  int n = 0;
  pp = fix16_sadd(fix16_sadd(fix16_smul(f->a,f->prior[n+1]),
                  fix16_smul(f->b,fix16_sadd(f->prior[n],f->prior[n+2]))),
                  f->c);
  y_hat = fix16_smul(
            fix16_sdiv(fix16_one,fix16_exp(fix16_smul(in,f->inv_x_n[n]))),
            f->inv_x_n[n]);
  f->posterior[n] = fix16_smul(pp,y_hat);

  if (f->posterior[n] < 0)
    f->posterior[n] = 0;
  sum = f->posterior[n];

  // Interior points
  for (int n=1; n<f->N-1; n++) {
    pp = fix16_sadd(fix16_sadd(fix16_smul(f->a,f->prior[n]),
                  fix16_smul(f->b,fix16_sadd(f->prior[n-1],f->prior[n+1]))),
                  f->c);
    y_hat = fix16_smul(
              fix16_sdiv(fix16_one,fix16_exp(fix16_smul(in,f->inv_x_n[n]))),
              f->inv_x_n[n]);
    f->posterior[n] = fix16_smul(pp,y_hat);
    if (f->posterior[n] < 0)
      f->posterior[n] = 0;
    sum = sum + f->posterior[n];
  }

  // Last end point
  n = f->N-1;
  pp = fix16_sadd(fix16_sadd(fix16_smul(f->a,f->prior[n-1]),
                fix16_smul(f->b,fix16_sadd(f->prior[n-2],f->prior[n]))),
                f->c);
  y_hat = fix16_smul(
            fix16_sdiv(fix16_one,fix16_exp(fix16_smul(in,f->inv_x_n[n]))),
            f->inv_x_n[n]);
  f->posterior[n] = fix16_smul(pp,y_hat);
  if (f->posterior[n] < 0)
    f->posterior[n] = 0;
  sum = sum + f->posterior[n];

  // Normalize
  int m = 0;
  fix16_t mlh = f->posterior[0];
  for (int n=0; n<f->N; n++){
    if (fix16_ssub(f->posterior[n],mlh) > 0){
      m = n;
      mlh = f->posterior[m];
    }
    f->prior[n] = fix16_smul(f->posterior[n],fix16_sdiv(f16_1000,sum));
  }
  return f->x_n[m];
}
