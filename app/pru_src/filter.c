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
fix16_t FiltIir(fix16_t in, iir_filt_t* filt) {
  for(int i=filt->N; i>0; i--){
    filt->x[i] = filt->x[i-1];
    filt->y[i] = filt->y[i-1];
  }

  filt->x[0] = in;

  fix16_t out = fix16_smul(filt->b[0], filt->x[0]);
  for(int k=1; k<filt->N+1; k++) {
    out = fix16_sadd(out,fix16_ssub(
                          fix16_smul(filt->b[k], filt->x[k]),
                          fix16_smul(filt->a[k], filt->y[k])));
  }
  filt->y[0] = out;
  return out;
}

// ----------------------------------------------------------------------------
fir_filt_t* FiltFirInit(const uint32_t filtorder, const fix16_t* b_coeff) {
  fir_filt_t* filt = malloc(sizeof(fir_filt_t));
  filt->N = filtorder;
  filt->b = malloc(sizeof(fix16_t)*(filtorder+1));
  filt->x = malloc(sizeof(fix16_t)*(filtorder+1));
  for (int i=0; i<filtorder+1; i++) {
    filt->x[i] = 0;
    filt->b[i] = b_coeff[i];
  }
  return filt;
}

// ----------------------------------------------------------------------------
fix16_t FiltFir(fix16_t in, fir_filt_t* filt) {
  for(int i=filt->N; i>0; i--){
    filt->x[i] = filt->x[i-1];
  }

  filt->x[0] = in;

  fix16_t out = fix16_smul(filt->b[0], filt->x[0]);
  for(int k=1; k<filt->N+1; k++) {
    out = fix16_sadd(out, fix16_smul(filt->b[k], filt->x[k]));
  }
  return out;
}

// ----------------------------------------------------------------------------
nlb_filt_t* FiltNlbInit(const uint32_t nbins,
                        const fix16_t fs,
                        const fix16_t max,
                        const fix16_t alpha,
                        const fix16_t beta){

  nlb_filt_t* filt = malloc(sizeof(nlb_filt_t));
  filt->x_n = malloc(sizeof(fix16_t)*nbins);
  filt->prior = malloc(sizeof(fix16_t)*nbins);
  filt->posterior = malloc(sizeof(fix16_t)*nbins);
  filt->N = nbins;

  filt->kappa = fix16_sdiv(fix16_smul(fix16_smul(
                fix16_from_int(nbins),fix16_from_int(nbins)),alpha),fs);
  filt->eta = fix16_sdiv(beta,fs);

  // Minimum parameters
  if (fix16_ssub(filt->eta,0x1) < 0) {
    filt->eta = (fix16_t)0x1;
  }
  if (fix16_ssub(filt->kappa,0x1) < 0) {
    filt->kappa = (fix16_t)0x1;
  }

  printf("kappa = %f\t eta = %f\n",
        fix16_to_float(filt->kappa),fix16_to_float(filt->eta));

  for (int i=0; i<nbins; i++) {
    filt->prior[i] = fix16_sdiv(fix16_from_int(1000),fix16_from_int(nbins));
    filt->posterior[i] = (fix16_t)0;
    filt->x_n[i] = fix16_smul(fix16_sdiv(max,fix16_from_int(nbins)),
                              fix16_from_int(i+1));
    printf("%i\t%f\t%f\t%f\n",i,
                          fix16_to_float(filt->x_n[i]),
                          fix16_to_float(filt->prior[i]),
                          fix16_to_float(
                            fix16_sdiv(fix16_one,filt->x_n[i])));
  }
  return filt;
}

// ----------------------------------------------------------------------------
fix16_t FiltNlb(fix16_t in, nlb_filt_t* filt){
  fix16_t d2p, jump, y_hat, sum;

  // 0xFFFE0000 = -2

  printf("prior\t\td2p\t\tjump\t\tyhat\t\tpost\t\tsumpost\n");

  // First end point
  int n = 0;
  d2p =
    fix16_smul(
        fix16_sadd(
          fix16_sadd(
            fix16_smul(0xFFFE0000,filt->prior[n+1]),
          filt->prior[n+2]),
        filt->prior[n]),
      filt->kappa);
  jump = fix16_smul(fix16_ssub(fix16_one,filt->prior[n]),filt->eta);
  y_hat = fix16_sdiv(fix16_exp(fix16_sdiv(-in,filt->x_n[n])),filt->x_n[n]);

  filt->posterior[n] =
    fix16_smul(fix16_sadd(fix16_sadd(filt->prior[n],d2p),jump),y_hat);
  if (filt->posterior[n] < 0)
    filt->posterior[n] = 0;
  sum = filt->posterior[n];

  printf("%f\t%f\t%f\t%f\t%f\t%f\n",
    fix16_to_float(filt->prior[n]),fix16_to_float(d2p), fix16_to_float(jump),
     fix16_to_float(y_hat), fix16_to_float(filt->posterior[n]),
     fix16_to_float(sum));


  // Interior points
  for (int n=1; n<filt->N-1; n++) {
    d2p =
      fix16_smul(
          fix16_sadd(
            fix16_sadd(
              fix16_smul(0xFFFE0000,filt->prior[n]),
            filt->prior[n-1]),
          filt->prior[n+1]),
        filt->kappa);
    jump = fix16_smul(fix16_ssub(fix16_one,filt->prior[n]),filt->eta);
    y_hat = fix16_sdiv(fix16_exp(fix16_sdiv(-in,filt->x_n[n])),filt->x_n[n]);

    filt->posterior[n] =
      fix16_smul(fix16_sadd(fix16_sadd(filt->prior[n],d2p),jump),y_hat);
    if (filt->posterior[n] < 0)
      filt->posterior[n] = 0;
    sum = sum + filt->posterior[n];

    printf("%f\t%f\t%f\t%f\t%f\t%f\n",
      fix16_to_float(filt->prior[n]),fix16_to_float(d2p), fix16_to_float(jump),
       fix16_to_float(y_hat), fix16_to_float(filt->posterior[n]),
       fix16_to_float(sum));
  }



  // Last end point
  n = filt->N-1;
  d2p =
    fix16_smul(
        fix16_sadd(
          fix16_sadd(
            fix16_smul(0xFFFE0000,filt->prior[n-1]),
          filt->prior[n-2]),
        filt->prior[n]),
      filt->kappa);
  jump = fix16_smul(fix16_ssub(fix16_one,filt->prior[n]),filt->eta);
  y_hat = fix16_sdiv(fix16_exp(fix16_sdiv(-in,filt->x_n[n])),filt->x_n[n]);

  filt->posterior[n] =
    fix16_smul(fix16_sadd(fix16_sadd(filt->prior[n],d2p),jump),y_hat);
  if (filt->posterior[n] < 0)
    filt->posterior[n] = 0;
  sum = sum + filt->posterior[n];


  printf("%f\t%f\t%f\t%f\t%f\t%f\n",
    fix16_to_float(filt->prior[n]),fix16_to_float(d2p), fix16_to_float(jump),
     fix16_to_float(y_hat), fix16_to_float(filt->posterior[n]),
     fix16_to_float(sum));

  int m = 0;
  fix16_t mlh = filt->posterior[0];
  for (int n=0; n<filt->N; n++){
    if (fix16_ssub(filt->posterior[n],mlh) > 0){
      m = n;
      mlh = filt->posterior[m];
    }
    filt->prior[n] = fix16_smul(filt->posterior[n],
                                fix16_sdiv(fix16_from_int(1000),sum));
  }

  printf("\nMAP =\n");
  printf("%f\n",fix16_to_float(filt->x_n[m]));

  return filt->x_n[m];
}
