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

volatile register uint32_t __R30;
volatile register uint32_t __R31;

motor_t* motor;
encoder_t* encoder;

uint32_t flag, t0, t;
const uint32_t Td_default = 30000;
const uint32_t Np_default = 0;
const fix16_t fo = 0x28F; // 0.001 Hz

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
  mem->p->Td = Td_default;
  mem->p->Np = Np_default;
  flag = 0;
  encoder = EncoderInit(0x1);
  motor = MaxonMotorInit(4,           // enable pin
                         0,           // adc cur ch
                         1,           // adc vel ch
                         0x240000,    // gear ratio (31/1)
                         0x198000,    // torque constant (25.5 mNm/A)
                         0x1760000,   // speed constant (374 rpm/V)
                         0x20000,     // max current (2 A)
                         0x4173290,   // max velocity (10000 rpm ~1047 rad/s)
                         0xF63C0000,  // slope (-10000/4)
                         0x13880000   // bias (5000)
                         );
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  EncoderUpdate(encoder);
  MaxonUpdate(motor);

  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
      flag = 1;
    }
		fix16_t tt = fix16_sdiv(fix16_from_int((c->frame - t0)),
                            fix16_from_int(p_->fs_hz));
    fix16_t freq_scaler = fix16_sadd(fix16_sdiv(tt,fix16_from_int(24)),fo);
	  MaxonSetCurrent(motor,fix16_sin(
                        fix16_smul(fix16_smul(fix16_from_int(2),fix16_pi),
                        fix16_smul(freq_scaler,tt))));
		if ((c->frame-t0) == p_->Td) {
      flag = 0;
      PruClearCtlBit(ctl_,0);
    }
	} else {
    MaxonSetCurrent(motor,0);
  }
  MaxonAction(motor);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);

}

void Pru1Cleanup(void) {
  MaxonMotorFree(motor);
}

