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
i2c_t* i2c2;
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
const uint32_t refract_cnts = 100;

// Thumbs up stuff
thumbsup_trigger_t* thumbsup;
iir_filt_t* mvavg;
//const fix16_t b_mvavg[2] = {0x21, 0};                    // [0.0005, 0]
//const fix16_t a_mvavg[2] = {fix16_one, 0xFFFF0021};      // [1 -(1-0.0005)]

// DC Blocking
const fix16_t b_hp[2] = {fix16_one, -fix16_one};
const fix16_t a_hp[2] = {fix16_one, 0xFFFF028F};  // -0.99

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
  mem->p->p_sense = 0x1E0000;     // 15 psi
  mem->p->p_pro = 0x280000;       // 40 psi
  mem->p->p_sup = 0x280000;       // 40 psi
  mem->p->thr_sup = -0x8000;      // -0.5
  mem->p->thr_pro = 0x8000;       // 0.5
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
  s_->buttons = InputButtonGetState(bgreen) - InputButtonGetState(bred);

  // exit for red button
  if (InputButtonGetState(bred))
    PruSetCtlBit(ctl_, 1);

}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

  i2c2 = I2cInit(2);
  mux = MuxI2cInit(i2c2,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,4,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,3,0x28), 9, 5, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam1,mem->p->p_sense);

  pam2 = PamInitMuscle(PressureSensorInit(mux,6,0x28), 6, 7, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam2,mem->p->p_sense);

  thumbsup = ThumbsUpTriggerInit(pam1, pam2, FiltIirInit(1, b_hp, a_hp));

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
  ThumbsUpTriggerUpdate(thumbsup, p_->hold_cnt,
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
  s_->triggersignal = ThumbsUpTriggerGetTriggerSig(thumbsup);
  s_->thumbsfsm = ThumbsUpTriggerGetState(thumbsup);
  s_->sync = SyncOutState(sync);


}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

