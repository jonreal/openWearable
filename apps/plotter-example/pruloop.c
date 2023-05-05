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

volatile register uint32_t __R30;
volatile register uint32_t __R31;

const fix16_t k2pi =  0x6487F; // 2*pi
const fix16_t dt = 0x42; // 1/1000
fix16_t t1 = 0;
fix16_t t2 = 0;
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

  t1 = fix16_smul(fix16_from_int(c->frame),dt);
  s_->pru0var = fix16_sin(fix16_smul(k2pi,t1));

}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
  debug_buff[0] = 0xFF;
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  // Pulse a few pins
  if ((c->frame % 50) == 0) {
    __R30 ^= (1 << 0);
    __R30 ^= (1 << 1);
  }
  t2 = fix16_smul(fix16_from_int(c->frame),dt);
  s_->pru1var = fix16_sin(fix16_smul(fix16_smul(k2pi,fix16_from_int(4)),t2));
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  debug_buff[1] = 0xA;
}

void Pru1Cleanup(void) {
}

