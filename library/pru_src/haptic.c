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

#include "haptic.h"
#include <stdlib.h>


hapitic_t* HapticInit(motor_t* motor, encoder_t* encoder,
                      iir_filt_t* lp1, iir_filt_t* lp2,
                      fix16_t lp_cutoff) {
  hapitic_t* h = malloc(sizeof(motor_t));
  h->m = motor;
  h->enc = encoder;
  h->lp1 = lp1;
  h->lp2 = lp2;
  h->lp_cutoff = lp_cutoff;
  h->tau_active = 0;
  h->dtheta = 0;
  return h;
}

void HapticUpdate(hapitic_t* h,
                  fix16_t Jvirt, fix16_t Bvirt, fix16_t Kvirt,
                  fix16_t dtheta0, fix16_t theta0) {
  fix16_t angle = EncoderGetAngle(h->enc);
  h->dtheta = fix16_ssub(angle,FiltIir(angle,h->lp1));
  h->ddtheta = fix16_ssub(h->dtheta,FiltIir(h->dtheta,h->lp2));
  h->tau_active = fix16_sadd(fix16_sadd(
                    fix16_smul(Jvirt,h->ddtheta),
                    fix16_smul(Bvirt,fix16_ssub(h->dtheta,dtheta0))),
                    fix16_smul(Kvirt,fix16_ssub(angle,theta0)));
  MaxonSetCurrent(h->m,h->tau_active);
}
