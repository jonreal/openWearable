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
#include "pam.h"
#include "filtcoeff.h"
#include "reflex.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pams
i2cmux_t* mux;
i2c_t* i2c;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
reflex_t* reflex;

// Not connected, only sensor
pam_t* pam3;
pam_t* pam4;

// DC blocking filter
const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF3333};  // -0.80

const uint32_t refractory = 150;

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

  // i2c 2
  // SDA - p9.20
  // SCL - p9.19
  i2c = I2cInit(2);
  mux = MuxI2cInit(i2c,0x70,PCA9548);

  // Ch. 8, 8-1 = 7
  reservoir = PamReservoirInit(PressureSensorInit(mux,7,0x28));


  // pam1
  // sensor on mux ch. 6, 6 - 1 = 5
  // out: P8.45, MODE5, pr1_pru1_pru_r30_0
  // in: 8.46, MODE5, pr1_pru1_pru_r30_1
  pam1 = PamInitMuscle(PressureSensorInit(mux,5,0x28),
                        reservoir,
                        0, 1,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,0);

  // pam2
  // sensor on mux ch. 5, 5 - 1 = 4
  // out: P8.43, MODE5, pr1_pru1_pru_r30_2
  // in: 8.44, MODE5, pr1_pru1_pru_r30_3
  pam2 = PamInitMuscle(PressureSensorInit(mux,4,0x28),
                        reservoir,
                        2, 3,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,0);

  reflex = ReflexInit(pam1,pam2,fix16_from_int(5), fix16_from_int(95),
                      FiltIirInit(1, b_dcblck, a_dcblck));


  // pam3
  // sensor on mux ch. 4, 4 - 1 = 3
  // out: NC
  // in: NC
  pam3 = PamInitMuscle(PressureSensorInit(mux,3,0x28),
                        reservoir,
                        4, 5,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam3,0);

  // pam4
  // sensor on mux ch. 3, 3 - 1 = 2
  // out: NC
  // in: NC
  pam4 = PamInitMuscle(PressureSensorInit(mux,2,0x28),
                        reservoir,
                        6, 7,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam4,0);

}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamReservoirUpdate(reservoir);

  PamUpdate(pam1);
  PamUpdate(pam2);
  PamUpdate(pam3);
  PamUpdate(pam4);

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

  ReflexUpdate(reflex, p_->threshold, p_->dP, 0);

  if (PruGetCtlBit(ctl_,2)) {
    PamSetPd(pam1,p_->P0);
    PamSetPd(pam2,p_->P0);
    PruClearCtlBit(ctl_,2);
  }

  PamActionSimple(pam1);
  PamActionSimple(pam2);

  s_->p_res = PamReservoirGetPressure(reservoir);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->pam3_state = PamGetState(pam3);
  s_->pam4_state = PamGetState(pam4);
  s_->triggersignal = reflex->triggersignal;

}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

