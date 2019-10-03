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

#ifndef _THUMBSUP_H_
#define _THUMBSUP_H_

#include <stdint.h>
#include "input.h"
#include "pam.h"

extern volatile uint32_t *debug_buff;

typedef enum {
  PRONATE = -1,
  NEUTRAL = 0,
  SUPINATE = 1,
  RECOVER = 2,
} fsm_state_t;

typedef struct {
  uint32_t cnt;
  fsm_state_t state;
  button_t* b_pro;
  button_t* b_sup;
  pam_t* pam_pro;
  pam_t* pam_sup;
} thumbsup_t;


typedef struct {
  uint32_t cnt;
  volatile fix16_t triggersignal;
  iir_filt_t* filt;
  pam_t* pam_pro;
  pam_t* pam_sup;
  volatile uint32_t flag;
  volatile fix16_t pm1_0;
  volatile fix16_t pm2_0;
  volatile fsm_state_t state;
} thumbsup_trigger_t;

// --- Button based
thumbsup_t* ThumbsUpInit(button_t* b_pro, button_t* b_sup,
                          pam_t* pam_pro, pam_t* pam_sup);
void ThumbsUpUpdate(thumbsup_t* th, uint32_t hold_cnt,
                    fix16_t pd_pro, fix16_t pd_sup);
fsm_state_t ThumbsUpGetState(thumbsup_t* th);
void ThumbsUpFree(thumbsup_t* th);

// --- Trigger based
thumbsup_trigger_t* ThumbsUpTriggerInit(pam_t* pam_pro, pam_t* pam_sup,
                          iir_filt_t* filt);
void ThumbsUpTriggerUpdate(thumbsup_trigger_t* th,
                                uint32_t hold_cnt,
                                fix16_t pd_sup, fix16_t pd_pro,
                                fix16_t sup_thrs, fix16_t pro_thrs,
                                fix16_t p_sens);
fix16_t ThumbsUpTriggerGetTriggerSig(thumbsup_trigger_t* th);
fsm_state_t ThumbsUpTriggerGetState(thumbsup_trigger_t* th);

#endif
