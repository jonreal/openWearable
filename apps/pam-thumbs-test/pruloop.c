/* Copyright 2017-2019 Jonathan Realmuto

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

#include "pruloop.h"
#include "thumbsup.h"
#include "sync.h"
#include "input.h"
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pam stuff
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
const uint32_t refract_cnts = 100;

// Thumbs up stuff
thumbs_up_interaction_t* thumbsup;
iir_filt_t* mvavg;
const fix16_t b_mvavg[2] = {0x21, 0};                    // [0.0005, 0]
const fix16_t a_mvavg[2] = {fix16_one, 0xFFFF0021};      // [1 -(1-0.0005)]

// Sync stuff
sync_t* sync;
const uint8_t sync_pin = 11;

// Button Stuff
button_t* bred;
button_t* bgreen;
const uint8_t bred_pin = 14;
const uint8_t bgreen_pin = 15;
const uint8_t debounce = 100;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  mem->p->p_sense = 0xF0000;    // 15 psi
  mem->p->p_pro = 0x280000;     // 40 psi
  mem->p->p_sup = 0x280000;     // 40 psi
  mem->p->thr_sup = -fix16_one;
  mem->p->thr_pro = fix16_one;
  mem->p->hold_cnt = 1000;

  bred = InputButtonInit(bred_pin, debounce);
  bgreen = InputButtonInit(bgreen_pin, debounce);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
  InputButtonUpdate(bred);
  InputButtonUpdate(bgreen);
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){

  s_->buttons = InputButtonGetState(bgreen) - InputButtonGetState(bred);

}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

  mux = MuxI2cInit(0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,4,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,6,0x28), 5, 4, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam1,mem->p->p_sense);

  pam2 = PamInitMuscle(PressureSensorInit(mux,3,0x28),7, 6, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam2,mem->p->p_sense);

  thumbsup = ThumbsUpInteractionInit(pam1, pam2,
                                  FiltIirInit(1, b_mvavg, a_mvavg));

  sync = SyncInitChan(sync_pin);

  SyncOutLow(sync);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {


  PamReservoirUpdate(reservoir);
  PamUpdate(pam1);
  PamUpdate(pam2);
  ThumbsUpInteractionUpdate(thumbsup, p_->hold_cnt,
                         p_->p_sup,
                         p_->p_pro,
                         p_->thr_sup,
                         p_->thr_pro,
                         p_->p_sense);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {


  PamActionSimple(pam1);
  PamActionSimple(pam2);

  if (PruGetCtlBit(ctl_,0))
    SyncOutHigh(sync);
  else
    SyncOutLow(sync);
  
  s_->p_res = PamReservoirGetPressure(reservoir);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->triggersignal = ThumbsUpInteractionGetTriggerSig(thumbsup);
  s_->thumbsfsm = ThumbsUpInteractionGetState(thumbsup);
  s_->sync = SyncOutState(sync);


}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

