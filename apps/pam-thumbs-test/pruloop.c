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
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;


// Pam stuff
i2cmux_t* mux;

reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
const uint32_t refract_cnts = 250;
const fix16_t p_default = 0xF0000; // 10 psi

// Thumbs up stuff
thumbs_up_interaction_t* thumbsup;
const fix16_t mv_alpha = 0x21;
const fix16_t pro_thrshld = fix16_one;
const fix16_t sup_thrshld = -fix16_one;

iir_filt_t* mvavg;
const fix16_t b_mvavg[2] = {0x21, 0};                    // [0.0005, 0]
const fix16_t a_mvavg[2] = {fix16_one, 0xFFFF0021};      // [1 -(1-0.0005)]

// Tracking
const uint32_t Td = 5000;
uint32_t t0, t;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {


  // init params
  mem->p->pd = p_default;
  t0 = 0;
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
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

  mux = MuxI2cInit(0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,4,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,6,0x28), 5, 4, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam1,p_default);

  pam2 = PamInitMuscle(PressureSensorInit(mux,3,0x28),7, 6, refract_cnts,
                        FiltIirInit(1, k_lp_1_2Hz_b, k_lp_1_2Hz_a));
  PamSetPd(pam2,p_default);

  thumbsup = ThumbsUpInteractionInit(pam1, pam2,
                                  FiltIirInit(1, b_mvavg, a_mvavg));
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamUpdate(pam1);
  PamUpdate(pam2);

  if (c->frame > 5000) {


    ThumbsUpInteractionUpdate(thumbsup, 3000,
                           0x280000, // 40 psi
                           0x280000, // 40 psi
                           -fix16_one,
                           fix16_one,
                           0xF0000);
  }


//  // update pd if param changed
//  if (PruGetCtlBit(ctl_, 1)) {
//    PamSetPd(pam1,p_->pd);
//    PamSetPd(pam2,p_->pd);
//    PruClearCtlBit(ctl_,1);
//  }
//  t = (c->frame - t0)*p_->fs_hz / (Td - Td/p_->fs_hz);
//  fix16_t pd_ =  fix16_sadd(
//    fix16_sdiv(fix16_from_int((int32_t)l_->lut[t % 1000]),fix16_from_int(100)),
//    fix16_from_int(25));
//  PamSetPd(pam1,pd_);
//  PamSetPd(pam2,pd_);


//




}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {


  PamReservoirUpdate(reservoir);
  s_->p_res = PamReservoirGetPressure(reservoir);

  PamActionSimple(pam1);
  PamActionSimple(pam2);

  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->triggersignal = ThumbsUpInteractionGetTriggerSig(thumbsup);
  s_->thumbsfsm = ThumbsUpInteractionGetState(thumbsup);
}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

