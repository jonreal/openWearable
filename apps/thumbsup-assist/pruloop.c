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
#include "reflex.h"
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
const uint32_t refract_cnts = 350;

// Reflex stuff
reflex_t* reflex;
// DC Blocking
const fix16_t b_hp[2] = {fix16_one, -fix16_one};
const fix16_t a_hp[2] = {fix16_one, 0xFFFF028F};  // -0.99

// Sync stuff
sync_t* sync;
const uint8_t sync_pin = 11;
const uint8_t led_sync_pin = 10;

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
  mem->p->p_0 = 0x280000;   // 40 psi
  mem->p->thr = 0x4000;     // 0.1
  mem->p->dp =  0x80000;

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
  PamSetPd(pam1,mem->p->p_0);

  pam2 = PamInitMuscle(PressureSensorInit(mux,6,0x28), 6, 7, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam2,mem->p->p_0);

  reflex = ReflexInit(pam1, pam2, fix16_from_int(15),
                      FiltIirInit(1, b_hp, a_hp));

  sync = SyncInitChan(sync_pin);
  SyncOutLow(sync);
  __R30 &= ~(1 << led_sync_pin);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
  PamReservoirUpdate(reservoir);
  PamUpdate(pam1);
  PamUpdate(pam2);

  ReflexUpdate(reflex, p_->thr, p_->dp);

  if (PruGetCtlBit(ctl_,2)) {
    PamSetPd(pam1,p_->p_0);
    PamSetPd(pam2,p_->p_0);
    PruClearCtlBit(ctl_,2);
  }

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {


  PamActionSimple(pam1);
  PamActionSimple(pam2);

  if (PruGetCtlBit(ctl_,0)) {
    SyncOutHigh(sync);
    __R30 |= (1 << led_sync_pin);
  } else {
    SyncOutLow(sync);
    __R30 &= ~(1 << led_sync_pin);
  }

  s_->p_res = PamReservoirGetPressure(reservoir);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->triggersignal = reflex->triggersignal; 
  s_->sync = SyncOutState(sync);


}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

