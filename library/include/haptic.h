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

#ifndef _HAPTIC_H_
#define _HAPTIC_H_

#include "fix16.h"
#include "maxon.h"
#include "filter.h"
#include "encoder.h"

typedef struct{
  volatile fix16_t dtheta;
  volatile fix16_t ddtheta;
  volatile fix16_t tau_active;
  fix16_t lp_cutoff;
  iir_filt_t* lp1;
  iir_filt_t* lp2;
  motor_t* m;
  encoder_t* enc;
} hapitic_t;


hapitic_t* HapticInit(motor_t* motor, encoder_t* encoder,
                      iir_filt_t* lp1, iir_filt_t* lp2,
                      fix16_t lp_cutoff);
void HapticUpdate(hapitic_t* h,
                  fix16_t Jvirt, fix16_t Bvirt, fix16_t Kvirt,
                  fix16_t dtheta0, fix16_t theta0);

#endif
