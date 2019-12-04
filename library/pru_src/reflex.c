/* Copyright 2019 Jonathan Realmuto

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

#include "reflex.h"
#include <stdlib.h>

reflex_t* ReflexInit(pam_t* pam_1, pam_t* pam_2,
                      fix16_t p_min, fix16_t p_max,
                      iir_filt_t* filt) {
  reflex_t* reflex = malloc(sizeof(reflex_t));
  reflex->flag = 0;
  reflex->pam_1 = pam_1;
  reflex->pam_2 = pam_2;
  reflex->triggersignal = 0;
  reflex->filt = filt;
  reflex->pm1_0 = 0;
  reflex->pm2_0 = 0;
  reflex->p_min = p_min;
  reflex->p_max = p_max;
  return reflex;
}

reflex_myo_t* ReflexMyoInit(pam_t* pam_1, pam_t* pam_2,
                      emg_t* emg_1, emg_t* emg_2,
                      fix16_t p_min, fix16_t p_max) {
  reflex_myo_t* reflex = malloc(sizeof(reflex_myo_t));
  reflex->flag = 0;
  reflex->pam_1 = pam_1;
  reflex->pam_2 = pam_2;
  reflex->emg_1 = emg_1;
  reflex->emg_2 = emg_2;
  reflex->triggersignal = 0;
  reflex->p_min = p_min;
  reflex->p_max = p_max;
  return reflex;
}



void ReflexUpdate(reflex_t* reflex, fix16_t threshold,
                    fix16_t delta, fix16_t ref) {

  fix16_t activation;

  if ((reflex->pam_2->fsm == HOLD) && (reflex->pam_1->fsm == HOLD)) {
    if (reflex->flag == 0) {
      reflex->pm1_0 = reflex->pam_1->s.pm;
      reflex->pm2_0 = reflex->pam_2->s.pm;
      activation = fix16_ssub(fix16_ssub(reflex->pm1_0,
                                        reflex->pam_1->s.pm),
                            fix16_ssub(reflex->pm2_0,
                                        reflex->pam_2->s.pm));
      reflex->filt->x[0] = activation;
      reflex->filt->x[1] = activation;
      reflex->filt->y[0] = activation;
      reflex->filt->y[1] = activation;
      reflex->triggersignal = FiltIir(activation,reflex->filt) + ref;
      reflex->flag = 1;
    } else {
      activation = fix16_ssub(fix16_ssub(reflex->pm1_0,
                                        reflex->pam_1->s.pm),
                            fix16_ssub(reflex->pm2_0,
                                        reflex->pam_2->s.pm));
      reflex->triggersignal = FiltIir(activation,reflex->filt) + ref;
    }

    // Compute logicals
    uint32_t pm1_min_sat = fix16_ssub(reflex->pam_1->s.pd,delta) > reflex->p_min;
    uint32_t pm1_max_sat = fix16_sadd(reflex->pam_1->s.pd,delta) < reflex->p_max;
    uint32_t pm2_min_sat = fix16_ssub(reflex->pam_2->s.pd,delta) > reflex->p_min;
    uint32_t pm2_max_sat = fix16_sadd(reflex->pam_2->s.pd,delta) < reflex->p_max;
    uint32_t pos = reflex->triggersignal > threshold;
    uint32_t neg = reflex->triggersignal < -threshold;
    uint32_t dpos = delta > 0;
    uint32_t dneg = delta < 0;

    if ( pos && ( (dpos && pm2_min_sat && pm1_max_sat)
               || (dneg && pm2_min_sat && pm1_max_sat))){
      PamSetPd(reflex->pam_1, fix16_sadd(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_ssub(reflex->pam_2->s.pd,delta));
    } else if  ( neg && ( (dpos && pm1_min_sat && pm2_max_sat)
               || (dneg && pm1_min_sat && pm2_max_sat))){
      PamSetPd(reflex->pam_1, fix16_ssub(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_sadd(reflex->pam_2->s.pd,delta));
    }
  } else {
    reflex->triggersignal = 0;
    reflex->flag = 0;
  }
}

void ReflexMyoUpdate(reflex_myo_t* reflex, fix16_t emg1, fix16_t emg2,
    fix16_t thres_u, fix16_t thres_l, fix16_t delta) {

  reflex->triggersignal = fix16_ssub(emg1,emg2);

  if ((reflex->pam_2->fsm == HOLD) && (reflex->pam_1->fsm == HOLD)) {
    // Compute logicals
    uint32_t pm1_min_sat = fix16_ssub(reflex->pam_1->s.pd,delta) > reflex->p_min;
    uint32_t pm1_max_sat = fix16_sadd(reflex->pam_1->s.pd,delta) < reflex->p_max;
    uint32_t pm2_min_sat = fix16_ssub(reflex->pam_2->s.pd,delta) > reflex->p_min;
    uint32_t pm2_max_sat = fix16_sadd(reflex->pam_2->s.pd,delta) < reflex->p_max;
    uint32_t pos = reflex->triggersignal > thres_u;
    uint32_t neg = reflex->triggersignal < thres_l;
    uint32_t dpos = delta > 0;
    uint32_t dneg = delta < 0;

    if ( pos && ( (dpos && pm2_min_sat && pm1_max_sat)
               || (dneg && pm2_min_sat && pm1_max_sat))){
      PamSetPd(reflex->pam_1, fix16_sadd(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_ssub(reflex->pam_2->s.pd,delta));
    } else if  ( neg && ( (dpos && pm1_min_sat && pm2_max_sat)
               || (dneg && pm1_min_sat && pm2_max_sat))){
      PamSetPd(reflex->pam_1, fix16_ssub(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_sadd(reflex->pam_2->s.pd,delta));
    }
  }
}
