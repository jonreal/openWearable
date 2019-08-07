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

reflex_t* ReflexInit(pam_t* pam_1, pam_t* pam_2, iir_filt_t* filt) {
  reflex_t* reflex = malloc(sizeof(reflex_t));
  reflex->flag = 0;
  reflex->pam_1 = pam_1;
  reflex->pam_2 = pam_2;
  reflex->triggersignal = 0;
  reflex->filt = filt;

  return reflex;
}
void ReflexUpdate(reflex_t* reflex, fix16_t threshold, fix16_t delta) {

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
      reflex->triggersignal = fix16_ssub(activation,
                                FiltIir(activation,reflex->filt));
      reflex->flag = 1;
    } else {
      activation = fix16_ssub(fix16_ssub(reflex->pm1_0,
                                        reflex->pam_1->s.pm),
                            fix16_ssub(reflex->pm2_0,
                                        reflex->pam_2->s.pm));
      reflex->triggersignal = fix16_ssub(activation,
                                FiltIir(activation,reflex->filt));
    }



    if (fix16_ssub(reflex->triggersignal,threshold) > 0) {
      PamSetPd(reflex->pam_1, fix16_sadd(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_ssub(reflex->pam_2->s.pd,delta));
    } else if (fix16_sadd(reflex->triggersignal,threshold) < 0)  {
      PamSetPd(reflex->pam_1, fix16_ssub(reflex->pam_1->s.pd,delta));
      PamSetPd(reflex->pam_2, fix16_sadd(reflex->pam_2->s.pd,delta));
    }
  } else {
    reflex->triggersignal = 0;
    reflex->flag = 0;
  }

}

