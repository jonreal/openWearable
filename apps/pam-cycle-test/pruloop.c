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
#include "sync.h"
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pam stuff
i2cmux_t* mux;
i2c_t* i2c1;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;

sync_t* sync;

fix16_t pd = 0;
uint32_t pulsecnt = 0;

const uint32_t refractory = 1;

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

  sync = SyncInitChan(4); // pru1.4

  i2c1 = I2cInit(2);
  mux = MuxI2cInit(i2c1,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,0,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,1,0x28),
                        reservoir,
                        0, 1,
                        0x1F8A034, // 504.6258 ms
                        0x2FAAD48, // 762.6769 ms
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,0);

  pam2 = PamInitMuscle(PressureSensorInit(mux,2,0x28),
                        reservoir,
                        2, 3,
                        0x1F96B36, // 505.4188 ms
                        0x33DD3A4, //829.8267 ms
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,0);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

  if (PruGetCtlBit(ctl_, 0)) {
    if (pulsecnt == 0) {
      pd = p_->Pmax;
      PamSetPd(pam2,pd);
    }
    else if (pulsecnt == p_->Ton) {
      pd = 0;
      PamSetPd(pam2,pd);
    }
    pulsecnt++;

    if ( ((pulsecnt >= p_->Tcheck) && (pulsecnt <= p_->Ton))
        && (s_->pam2_state.pm < p_->Pmin)) {
      pd = 0;
      PamSetPd(pam2,pd);
      PruClearCtlBit(ctl_,0);
    }
    if (pulsecnt == (p_->Ton + p_->Toff)){
      pulsecnt = 0;
    }

  }
  PamActionSimple(pam1);
  PamActionSimple(pam2);
  PamReservoirUpdate(reservoir);
  PamUpdate(pam1);
  PamUpdate(pam2);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->sync = SyncOutState(sync);

}

void Pru1Cleanup(void) {

  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);

}

