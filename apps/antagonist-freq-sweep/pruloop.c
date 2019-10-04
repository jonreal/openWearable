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
#include "pam.h"
#include "sync.h"
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

motor_t* motor;
encoder_t* encoder;

i2c_t* i2c1;
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
sync_t* sync;


uint32_t flag = 0;
uint32_t t0 = 0;
fix16_t t = 0;
fix16_t dt = 0;
fix16_t k = 0;

const uint32_t Tf_default = 240000;        // 240 s
const fix16_t f0_default = 0x28F;         // 0.01 Hz
const fix16_t f1_default = fix16_one;       // 2 Hz
const fix16_t A_default = 0x8000;         // 0.5 A
const fix16_t p0_default = 0x140000;      // 20 psi
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
  mem->p->Tf = Tf_default;
  mem->p->f0 = f0_default;
  mem->p->f1 = f1_default;
  mem->p->A = A_default;
  mem->p->p0 = p0_default;

  i2c1 = I2cInit(1);
  mux = MuxI2cInit(i2c1,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,0,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,1,0x28), 0, 1, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,mem->p->p0);

  pam2 = PamInitMuscle(PressureSensorInit(mux,2,0x28), 2, 3, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,mem->p->p0);

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


  sync = SyncInitChan(5);
  SyncOutLow(sync);

  k = fix16_sdiv(fix16_ssub(mem->p->f1,mem->p->f0),
                fix16_from_int(mem->p->Tf/mem->p->fs_hz));
  dt = fix16_sdiv(fix16_one,fix16_from_int(mem->p->fs_hz));
  debug_buff[0] = k;
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  EncoderUpdate(encoder);
  MaxonUpdate(motor);
  PamUpdate(pam1);
  PamUpdate(pam2);

  if (PruGetCtlBit(ctl_,2)) {
    k = fix16_sdiv(fix16_ssub(p_->f1,p_->f0),
                fix16_from_int(p_->Tf/p_->fs_hz));
    PruClearCtlBit(ctl_,2);
  }

  if (PruGetCtlBit(ctl_,1)) {
    PamSetPd(pam1,p_->p0);
    PamSetPd(pam2,p_->p0);
    PruClearCtlBit(ctl_,1);
  }

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

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  MaxonAction(motor);
  PamActionSimple(pam1);
  PamActionSimple(pam2);
  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->sync = SyncOutState(sync);
}

void Pru1Cleanup(void) {
  MaxonMotorFree(motor);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

