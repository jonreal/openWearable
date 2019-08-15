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
#include "encoder.h"
#include "sync.h"
#include "pwmdriver.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;


const uint32_t Td_default = 5000;
const uint32_t Np_default = 1;
uint32_t flag, t0, t;
const uint8_t sync_pin = 8;

sync_t* sync;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  mem->p->Td = Td_default;
  mem->p->Np = Np_default;
  flag = 0;
  debug_buff[0] = encoderInit();
  sync = SyncInitChan(sync_pin);
  SyncOutLow(sync);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
      SyncOutHigh(sync);
      flag = 1;
    }
    t = (c->frame - t0)*p_->fs_hz / (p_->Td - p_->Td/p_->fs_hz);
		s_->xd = fix16_ssub(fix16_smul(fix16_from_int(90),
						 	fix16_sadd(fix16_sdiv(
							fix16_from_int( (int32_t) l_->lut[t % 1000]),
							fix16_from_int(1000)),fix16_one)),fix16_from_int(90));
		if ((c->frame-t0) == (p_->Np*p_->Td)) {
      flag = 0;
      PruClearCtlBit(ctl_,0);
      SyncOutLow(sync);
    }
	} else {
    s_->xd = 0;
  }
  encoderSample(&s_->x);
  s_->vsync = (uint32_t)SyncOutState(sync);
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
  pwmInit();
  pwmSetCmpValue((uint16_t)5000);
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

}

void Pru1Cleanup(void) {
  pwmCleanUp();
}

