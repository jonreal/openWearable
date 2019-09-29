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

#include "encoder.h"
#include "sync.h"
#include "maxon.h"
#include "haptic.h"
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

const uint32_t Td_default = 5000;
const uint32_t Np_default = 1;
uint32_t flag, t0, t;

sync_t* sync;
motor_t* motor;
encoder_t* encoder;
hapitic_t* haptic;

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
  mem->p->bvirtual = 0;
  flag = 0;
  sync = SyncInitChan(5);
  SyncOutLow(sync);
  motor = MaxonMotorInit(4,           // enable pin
                         0,           // adc cur ch
                         1,           // adc vel ch
                         0x240000,    // gear ratio (31/1)
                         0x198000,    // torque constant (25.5 mNm/A)
                         0x1760000,   // speed constant (374 rpm/V)
                         0x40000,     // max current (4 A)
                         0x4173290,   // max velocity (10000 rpm ~1047 rad/s)
                         0x4E20000,   // slope (10000/8)
                         0x13880000   // bias (5000)
                         );
  encoder = EncoderInit(0x1);
//  EncoderTare(encoder);
  haptic = HapticInit(motor,encoder,
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      0x1F6A7A // 10pi (5 Hz)
                      );
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  EncoderUpdate(encoder);
  MaxonUpdate(motor);
  HapticUpdate(haptic, 0, p_->bvirtual, 0, 0, 0);

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
  s_->vsync = (uint32_t)SyncOutState(sync);

  if (c->frame > 1000)
    MaxonAction(motor);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);
  s_->dx = haptic->dtheta;
  s_->tau_active = haptic->tau_active;
}

void Pru1Cleanup(void) {
  MaxonMotorFree(motor);
}

