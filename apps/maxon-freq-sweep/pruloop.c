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
#include "maxon.h"
#include "encoder.h"
#include "sync.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

motor_t* motor;
encoder_t* encoder;
sync_t* sync;

uint32_t flag = 0;
uint32_t t0 = 0;
fix16_t t = 0;
fix16_t dt = 0;
fix16_t k = 0;


// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  //mem->p->Tf = Tf_default;
  //mem->p->f0 = f0_default;
  //mem->p->f1 = f1_default;
  //mem->p->A = A_default;

  encoder = EncoderInit(0x1);
  motor = MaxonMotorInit(6,           // enable pin pru0.6
                         0,           // adc cur ch
                         1,           // adc vel ch
                         0x240000,    // gear ratio (31/1)
                         0x198000,    // torque constant (25.5 mNm/A)
                         0x1760000,   // speed constant (374 rpm/V)
                         0x20000,     // max current (2 A)
                         0x4173290,   // max velocity (10000 rpm ~1047 rad/s)
                         0            // duty bias
                         );
  sync = SyncInitChan(5);
  SyncOutLow(sync);
  k = fix16_sdiv(fix16_ssub(mem->p->f1,mem->p->f0),
                fix16_from_int(mem->p->Tf/mem->p->fs_hz));
  dt = fix16_sdiv(fix16_one,fix16_from_int(mem->p->fs_hz));
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  EncoderUpdate(encoder);
  MaxonUpdate(motor);

  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
      SyncOutHigh(sync);
      flag = 1;
    }
		t += dt;
	  MaxonSetCurrent(motor,fix16_smul(p_->A,
               fix16_sin(fix16_smul(fix16_smul(fix16_smul(0x20000,fix16_pi),t),
                            fix16_sadd(fix16_smul(k,t),p_->f0)))));
		if ((c->frame-t0) == p_->Tf) {
      flag = 0;
      t = 0;
      t0 = 0;
      PruClearCtlBit(ctl_,0);
      SyncOutLow(sync);
    }
	} else {
    MaxonSetCurrent(motor,0);
  }
  MaxonAction(motor);



}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){

  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);
  s_->sync = SyncOutState(sync);

}

void Pru0Cleanup(void) {

  MaxonMotorFree(motor);
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

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

}

void Pru1Cleanup(void) {
}

