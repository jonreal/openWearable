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

#include "thumbsup.h"
#include <stdlib.h>

// --- Button Based

thumbs_up_t* ThumbsUpInit(button_t* b_pro, button_t* b_sup,
                          pam_t* pam_pro, pam_t* pam_sup) {
  thumbs_up_t* th = malloc(sizeof(thumbs_up_t));
  th->cnt = 0;
  th->state = NEUTRAL;

  th->b_pro = b_pro;
  th->b_sup = b_sup;
  th->pam_pro = pam_pro;
  th->pam_sup = pam_sup;

  PamSetPd(th->pam_pro,fix16_from_int(0));
  PamSetPd(th->pam_sup,fix16_from_int(0));
  PamActionSimple(th->pam_sup);
  PamActionSimple(th->pam_sup);

  return th;
}

void ThumbsUpUpdate(thumbs_up_t* th, uint32_t hold_cnt,
                    fix16_t pd_sup, fix16_t pd_pro) {

  // update button states
  InputButtonUpdate(th->b_pro);
  InputButtonUpdate(th->b_sup);
  uint8_t vbpro = InputButtonGetState(th->b_pro);
  uint8_t vbsup = InputButtonGetState(th->b_sup);

  // update finite state machine
  switch (th->state) {
    case NEUTRAL :
      if (vbpro) {
        th->state = PRONATE;
        th->cnt = 0;
        PamSetPd(th->pam_pro, pd_pro);
      } else if (vbsup) {
        th->state = SUPINATE;
        th->cnt = 0;
        PamSetPd(th->pam_sup, pd_sup);
      }
      break;
    case PRONATE :
      if (th->cnt >= hold_cnt) {
        th->state = NEUTRAL;
        PamSetPd(th->pam_pro, 0);
      }
      th->cnt++;
      break;
    case SUPINATE :
      if (th->cnt >= hold_cnt) {
        th->state = NEUTRAL;
        PamSetPd(th->pam_sup, 0);
      }
      th->cnt++;
      break;
  }

  PamActionSimple(th->pam_sup);
  PamActionSimple(th->pam_pro);
}

fsm_state_t ThumbsUpGetState(thumbs_up_t* th) {
  return th->state;
}
void ThumbsUpFree(thumbs_up_t* th) {
  PamMuscleFree(th->pam_sup);
  PamMuscleFree(th->pam_pro);
}





//fix16_t ThumbsUpGetSupPm(thumbs_up_t* th) {
//  return th->pam_sup->s.pm;
//}
//fix16_t ThumbsUpGetSupPd(thumbs_up_t* th) {
//  return th->pam_sup->s.pd;
//}
//fix16_t ThumbsUpGetProPm(thumbs_up_t* th) {
//  return th->pam_pro->s.pm;
//}
//fix16_t ThumbsUpGetProPd(thumbs_up_t* th) {
//  return th->pam_pro->s.pd;
//}
//
//


// --- Interaction base
thumbs_up_interaction_t* ThumbsUpInteractionInit(pam_t* pam_pro,
                                                  pam_t* pam_sup,
                                                  iir_filt_t* filt) {
  thumbs_up_interaction_t* th = malloc(sizeof(thumbs_up_interaction_t));
  th->cnt = 0;
  th->flag = 0;
  th->state = NEUTRAL;
  th->pam_pro = pam_pro;
  th->pam_sup = pam_sup;
  th->triggersignal = 0;
  th->filt = filt;
  th->pm1_0 = 0;
  th->pm2_0 = 0;

//  PamSetPd(th->pam_pro,fix16_from_int(0));
//  PamSetPd(th->pam_sup,fix16_from_int(0));
//  PamActionSimple(th->pam_sup);
//  PamActionSimple(th->pam_sup);

  return th;
}

void ThumbsUpInteractionUpdate(thumbs_up_interaction_t* th,
                                uint32_t hold_cnt,
                                fix16_t pd_sup, fix16_t pd_pro,
                                fix16_t sup_thrs, fix16_t pro_thrs,
                                fix16_t p_sens) {
  fix16_t activation;


  if ((th->pam_pro->fsm == HOLD) && (th->pam_sup->fsm == HOLD)) {

    if (th->flag == 0) {
      th->pm1_0 = th->pam_pro->s.pm;
      th->pm2_0 = th->pam_sup->s.pm;
      activation = fix16_ssub(fix16_ssub(th->pm1_0,
                                        th->pam_pro->s.pm),
                            fix16_ssub(th->pm2_0,
                                        th->pam_sup->s.pm));
      th->filt->x[0] = activation;
      th->filt->x[1] = activation;
      th->filt->y[0] = activation;
      th->filt->y[1] = activation;
      //th->triggersignal = fix16_ssub(activation,
      //                          FiltIir(activation,th->filt));
      th->triggersignal = FiltIir(activation,th->filt);
      th->flag = 1;
    } else {
      activation = fix16_ssub(fix16_ssub(th->pm1_0,
                                        th->pam_pro->s.pm),
                            fix16_ssub(th->pm2_0,
                                        th->pam_sup->s.pm));
//      th->triggersignal = fix16_ssub(activation,
//                                FiltIir(activation,th->filt));
//
      th->triggersignal = FiltIir(activation,th->filt);
    }

    switch (th->state) {
      case NEUTRAL :
        if (fix16_ssub(th->triggersignal,pro_thrs) > 0) {
          th->state = PRONATE;
          th->cnt = 0;
          PamSetPd(th->pam_pro, pd_pro);
          PamSetPd(th->pam_sup,0x20000);
        } else if (fix16_ssub(th->triggersignal,sup_thrs) < 0) {
          th->state = SUPINATE;
          th->cnt = 0;
          PamSetPd(th->pam_sup, pd_sup);
          PamSetPd(th->pam_pro, 0x20000);
        }
        break;

      case PRONATE :
        if (th->cnt >= hold_cnt) {
          th->state = RECOVER;
          th->cnt = 0;
          PamSetPd(th->pam_pro, 0x20000);
          PamSetPd(th->pam_sup, 0x20000);
        }
        th->cnt++;
        break;

      case SUPINATE :
        if (th->cnt >= hold_cnt) {
          th->state = RECOVER;
          th->cnt = 0;
          PamSetPd(th->pam_sup, 0x20000);
          PamSetPd(th->pam_pro, 0x20000);
        }
        th->cnt++;
        break;

      case RECOVER :
        if (th->cnt >= 500) {
          th->state = NEUTRAL;
          th->cnt = 0;
          PamSetPd(th->pam_sup, p_sens);
          PamSetPd(th->pam_pro, p_sens);
        }
        th->cnt++;
        break;
    }
  } else {
    th->triggersignal = 0;
    th->flag = 0;
  }
}


fix16_t ThumbsUpInteractionGetTriggerSig(thumbs_up_interaction_t* th) {
  return th->triggersignal;
}

fsm_state_t ThumbsUpInteractionGetState(thumbs_up_interaction_t* th) {
  return th->state;
}

