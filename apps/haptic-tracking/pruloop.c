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


uint32_t flag = 0;
uint32_t t0 = 0;
uint32_t t = 0;
fix16_t dt = 0;
uint32_t itarg = 0;
uint32_t debounce = 0;

sync_t* sync;
motor_t* motor;
encoder_t* encoder;
hapitic_t* haptic;

const uint32_t Td_default = 5000;
const uint32_t Np_default = 10;
const fix16_t f1_default = fix16_one;       // 2 Hz
const fix16_t A_default = 0x8000;         // 0.5 A

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {

  mem->p->Td = Td_default;
  mem->p->Np = Np_default;

  mem->p->Jvirtual = 0;
  mem->p->bvirtual = 0;
  mem->p->kvirtual = 0;

  flag = 0;
  sync = SyncInitChan(5);
  SyncOutLow(sync);
  motor = MaxonMotorInit(6,           // enable pin
                         0,           // adc cur ch
                         1,           // adc vel ch
                         0x240000,    // gear ratio (31/1)
                         0x198000,    // torque constant (25.5 mNm/A)
                         0x1760000,   // speed constant (374 rpm/V)
                         0x40000,     // max current (4 A)
                         0x4173290   // max velocity (10000 rpm ~1047 rad/s)
                         );
  encoder = EncoderInit(0x1);
  haptic = HapticInit(motor,encoder,
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      0x1F6A7A // 10pi (5 Hz)
                      );

  dt = fix16_sdiv(fix16_one,fix16_from_int(mem->p->fs_hz));

}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
  fix16_t theta0 = 0;

  EncoderUpdate(encoder);
  MaxonUpdate(motor);
  HapticUpdate(haptic, p_->Jvirtual, p_->bvirtual, p_->kvirtual, theta0, 0);
  //HapticPendulumUpdate(haptic, p_->Jvirtual, p_->bvirtual, p_->kvirtual);


  // Tracking
  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
      SyncOutHigh(sync);
      flag = 1;
    }
    //t = (c->frame - t0)*p_->fs_hz / (p_->Td - p_->Td/p_->fs_hz);
		if ((c->frame-t0) == (p_->Np*p_->Td)) {
      flag = 0;

      PruClearCtlBit(ctl_,0);
      SyncOutLow(sync);
    }
		//s_->xd = fix16_ssub(fix16_smul(fix16_from_int(90),
		//				 	fix16_sadd(fix16_sdiv(
		//					fix16_from_int( (int32_t) l_->lut[t % 1000]),
		//					fix16_from_int(1000)),fix16_one)),fix16_from_int(90));

    s_->xd = fix16_smul(0x5A0000, fix16_sin(fix16_smul(p_->k2PiFd,t)));
	}
  // Ballistic
  else if (PruGetCtlBit(ctl_, 1)) {
    if (flag == 0) {
      SyncOutHigh(sync);
      flag = 1;
    }
		if (itarg > (p_->Np-1)) {
      flag = 0;
      itarg = 0;
      PruClearCtlBit(ctl_,1);
      SyncOutLow(sync);
    }
    s_->xd = p_->targets[itarg];
    if ((fix16_ssub(fix16_ssub(s_->xd,s_->x),0x20000) < 0)
    && (fix16_sadd(fix16_ssub(s_->xd,s_->x),0x20000) > 0)){
      debounce++;
      if (debounce > 500) {
        itarg++;
        s_->xd = p_->targets[itarg];
        debounce = 0;
      }
    }
    theta0 = s_->xd;
  } else {
    s_->xd = 0;
    theta0 = s_->xd;
  }

  s_->vsync = (uint32_t)SyncOutState(sync);
  if (c->frame > 1000)
    MaxonAction(motor);

  t += dt;

}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){

  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);
  s_->dx = haptic->dtheta;
  s_->tau_active = haptic->tau_active;

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

