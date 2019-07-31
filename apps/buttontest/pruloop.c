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

// add drivers here
#include "thumbsup.h"
#include "sync.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Button stuff
const uint8_t pinb1 = 6;
const uint8_t pinb2 = 7;

// Pam stuff
//const uint8_t mux_i2c_add = 0x70;  // bbbw
const uint8_t mux_i2c_add = 0x77;  // bbblue
const uint8_t sensor_i2c_add = 0x28;
i2cmux_t* mux;

const uint8_t pam_pro_ch = 0;
const uint8_t pam_pro_hp_pin = 1;
const uint8_t pam_pro_lp_pin = 0;
const uint8_t pam_sup_ch = 1;
const uint8_t pam_sup_hp_pin = 3;
const uint8_t pam_sup_lp_pin = 2;

// thumbs up stuff
thumbs_up_t* th;

// sync
sync_t* sync;
const uint8_t syncpin = 4;

// default button params
const fix16_t pd_sup_default = 0x190000;  // 25 psi
const fix16_t pd_pro_default = 0x190000;  // 25 psi
const uint32_t hold_cnt_default = 3000;   // 3 sec

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
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

  // set default params
  mem->p->pd_sup = pd_sup_default;
  mem->p->pd_pro = pd_pro_default;
  mem->p->hold_cnt = hold_cnt_default;

  // i2c mux is needed for pams
  mux = MuxI2cInit(mux_i2c_add);

  pressure_sensor_t* ps = PressureSensorInit(mux, 1, sensor_i2c_add);
  PressureSensorSample(ps);
  __delay_cycles(100000);

//  pressure_sensor_t* ps2 = PressureSensorInit(mux, 3, sensor_i2c_add);
//  PressureSensorSample(ps2);
//  __delay_cycles(100000);
//


//  // setup thumbsup
//  th = ThumbsUpInit(
//        InputButtonInit(pinb1, 100),
//        InputButtonInit(pinb2, 100),
//        PamInitMuscle(PressureSensorInit(mux, pam_pro_ch, sensor_i2c_add),
//                                          pam_pro_hp_pin, pam_pro_lp_pin),
//        PamInitMuscle(PressureSensorInit(mux, pam_sup_ch, sensor_i2c_add),
//                                          pam_sup_hp_pin, pam_sup_lp_pin));
//
//  sync = SyncInitChan(4);
//  SyncOutLow(sync);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  // Check for sync
//  if (PruGetCtlBit(ctl_,1))
//    SyncOutHigh(sync);
//  else
//    SyncOutLow(sync);
//
//
//  ThumbsUpUpdate(th, p_->hold_cnt, p_->pd_pro, p_->pd_sup);
//
//  // Store results
//  s_->fsm_state = (int32_t) ThumbsUpGetState(th);
//  s_->pm_sup = ThumbsUpGetSupPm(th);
//  s_->pd_sup = ThumbsUpGetSupPd(th);
//  s_->pm_pro = ThumbsUpGetProPm(th);
//  s_->pd_pro = ThumbsUpGetProPd(th);
//  s_->sync = (uint32_t) SyncOutState(sync);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
}

void Pru1Cleanup(void) {
  ThumbsUpFree(th);
}


// Utility functions
uint8_t PruGetCtlBit(const pru_ctl_t* ctl_, uint8_t nbit) {
  return ((ctl_->bit.utility & (1 << nbit)) == (1 << nbit));
}

void PruSetCtlBit(pru_ctl_t* ctl_, uint8_t nbit) {
  ctl_->bit.utility |= (1 << nbit);
}

void PruClearCtlBit(pru_ctl_t* ctl_, uint8_t nbit) {
  ctl_->bit.utility &= ~(1 << nbit);
}
