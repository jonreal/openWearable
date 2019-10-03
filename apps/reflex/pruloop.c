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
#include <stdlib.h>

// Modules
#include "filter.h"
#include "filtcoeff.h"
#include "pam.h"
#include "reflex.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;


// PAMs
i2c_t* i2c1;
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
reflex_t* reflex;

// DC blocking filter
//const fix16_t b_mvavg[2] = {fix16_one, -fix16_one};
//const fix16_t a_mvavg[2] = {fix16_one, 0xFFFF0CCD};  // -0.95
const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF028F};  // -0.99

const uint32_t refractory = 150;
const fix16_t reflexthreshold = 0x2000; // 0.25
const fix16_t reflexdelta = 0x20000;

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
                     pru_ctl_t* ctl_)  {
}

void Pru0UpdateControl(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

  i2c1 = I2cInit(1);
  mux = MuxI2cInit(i2c1,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,0,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,1,0x28), 0, 1, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,fix16_from_int(35));

  pam2 = PamInitMuscle(PressureSensorInit(mux,2,0x28), 2, 3, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,fix16_from_int(35));

  reflex = ReflexInit(pam1,pam2,fix16_from_int(15),
                      FiltIirInit(1, b_dcblck, a_dcblck));
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamReservoirUpdate(reservoir);
  PamUpdate(pam1);
  PamUpdate(pam2);
  ReflexUpdate(reflex, reflexthreshold, reflexdelta);


}

void Pru1UpdateControl(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamActionSimple(pam1);
  PamActionSimple(pam2);

  // Store data
  s_->p_res = PamReservoirGetPressure(reservoir);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->triggersignal = reflex->triggersignal;
}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

