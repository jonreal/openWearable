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
#include "sync.h"
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
sync_t* sync;


// Default params
uint32_t kHoldtime = 5000;
uint32_t kWhichPam = 2;

uint32_t tick = 0;
uint32_t flag = 0;


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

  mem->p->holdtime = kHoldtime;
  mem->p->whichpam = kWhichPam;

  i2c1 = I2cInit(1);
  mux = MuxI2cInit(i2c1,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,0,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,1,0x28), 0, 1, 0,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,0);

  pam2 = PamInitMuscle(PressureSensorInit(mux,2,0x28), 2, 3, 0,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,0);

  sync = SyncInitChan(7);
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

}

void Pru1UpdateControl(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  if (PruGetCtlBit(ctl_,0)) {
    if (flag == 0) {
      SyncOutHigh(sync);
      tick = 0;
      flag = 1;
      switch (p_->whichpam) {
        case 0:
          __R30 |= (1 << pam1->hp_pin);
          pam1->s.u = 1;
          break;
        case 1:
         __R30 |= (1 << pam2->hp_pin);
          pam2->s.u = 1;
          break;
        case 2:
         __R30 |= (1 << pam1->hp_pin) | (1 << pam2->hp_pin);
          pam1->s.u = 1;
          pam2->s.u = 1;
          break;
      }
    } else if (tick == p_->holdtime) {
      __R30 = 0;
      pam1->s.u = 0;
      pam2->s.u = 0;
    } else if (tick == 2*p_->holdtime) {
      switch (p_->whichpam) {
        case 0:
          __R30 &= ~(1 << pam1->hp_pin);
          __R30 |= (1 << pam1->lp_pin);
          pam1->s.u = -1;
          break;
        case 1:
          __R30 &= ~(1 << pam2->hp_pin);
         __R30 |= (1 << pam2->lp_pin);
          pam2->s.u = -1;
          break;
        case 2:
         __R30 &= ~(1 << pam1->hp_pin) & ~(1 << pam2->hp_pin);
         __R30 |= (1 << pam1->lp_pin) | (1 << pam2->lp_pin);
          pam1->s.u = -1;
          pam2->s.u = -1;
          break;
      }
    } else if (tick == 3*p_->holdtime) {
      __R30 = 0;
      pam1->s.u = 0;
      pam2->s.u = 0;
      flag = 0;
      SyncOutLow(sync);
      PruClearCtlBit(ctl_, 0);
    }
    tick++;
  }


  // Store data
  s_->p_res = PamReservoirGetPressure(reservoir);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->sync = SyncOutState(sync);
}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

