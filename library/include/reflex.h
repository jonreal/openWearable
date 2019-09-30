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

#ifndef _REFLEX_H_
#define _REFLEX_H_


#include "pam.h"

typedef struct {
  volatile uint32_t flag;
  volatile fix16_t triggersignal;
  volatile fix16_t pm1_0;
  volatile fix16_t pm2_0;
  fix16_t p_sens;
  iir_filt_t* filt;
  pam_t* pam_1;
  pam_t* pam_2;
} reflex_t;

reflex_t* ReflexInit(pam_t* pam_1, pam_t* pam_2, fix16_t p_sens, iir_filt_t* filt);
void ReflexUpdate(reflex_t* reflex, fix16_t threshold, fix16_t delta);

#endif
