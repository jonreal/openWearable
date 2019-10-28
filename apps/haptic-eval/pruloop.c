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
#include "pam.h"
#include "reflex.h"
#include "filtcoeff.h"



volatile register uint32_t __R30;
volatile register uint32_t __R31;


// Haptic pru0
sync_t* sync;
motor_t* motor;
encoder_t* encoder;
hapitic_t* haptic;

uint32_t flag = 0;
uint32_t t0 = 0;
uint32_t t = 0;
uint32_t itarg = 0;
uint32_t debounce = 0;
fix16_t theta0 = 0xB40000;

const uint32_t Td_default = 10000;
const uint32_t Np_default = 1;
const fix16_t f1_default = fix16_one;       // 2 Hz
const fix16_t A_default = 0x8000;         // 0.5 A

// PAMs pru1
i2c_t* i2c1;
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;

reflex_t* reflex;
//reflex_myo_t* reflex;

emg_t* emg1;
emg_t* emg2;

fix16_t tt = 0;
fix16_t dt = 0;

// DC blocking filter
//const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
//const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF028F};  // -0.99

const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF3333};  // -0.80

const uint32_t refractory = 150;
const fix16_t reflexthreshold = 0x666; // 0.1
const fix16_t reflexdelta = 0x20000;
const fix16_t P0 = 0x280000; // 60 psi

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
  sync = SyncInitChan(7); // pru0.7
  SyncOutLow(sync);
  encoder = EncoderInit(0x1);
  motor = MaxonMotorInit(6,           // enable pin pru0.6
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
  haptic = HapticInit(motor,encoder,
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      0x1F6A7A // 10pi (5 Hz)
                      );

  emg1 = EmgInitSensor(0x0);
  emg2 = EmgInitSensor(0x1);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_){
  EncoderUpdate(encoder);
  MaxonUpdate(motor);
  HapticPendulumUpdate(haptic, p_->Jvirtual, p_->bvirtual,
      p_->kvirtual, theta0, 0, fix16_one);

  // must down sample adc
  if (c->frame % 2) {
    EmgUpdate(emg1);
  } else {
    EmgUpdate(emg2);
  }
  s_->emg1_state.bits = EmgGetBits(emg1);
  s_->emg2_state.bits = EmgGetBits(emg2);
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
  // Tracking
  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
      SyncOutHigh(sync);
      flag = 1;
    }
    t = (c->frame - t0)*p_->fs_hz / (p_->Td - p_->Td/p_->fs_hz);
		if ((c->frame-t0) == (p_->Np*p_->Td)) {
      flag = 0;
      PruClearCtlBit(ctl_,0);
      SyncOutLow(sync);
    }
		s_->xd = fix16_ssub(fix16_smul(fix16_from_int(90),
						 	fix16_sadd(fix16_sdiv(
							fix16_from_int( (int32_t) l_->lut[t % 1000]),
							fix16_from_int(1000)),fix16_one)),fix16_from_int(90));
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
    if ((fix16_ssub(fix16_ssub(s_->xd,s_->x),0x40000) < 0)
    && (fix16_sadd(fix16_ssub(s_->xd,s_->x),0x40000) > 0)){
      debounce++;
      if (debounce > 250) {
        itarg++;
        s_->xd = p_->targets[itarg];
        debounce = 0;
      }
    }
    theta0 = s_->xd;
  } else {
    s_->xd = 0;
    theta0 = 0;
  }

  s_->vsync = (uint32_t)SyncOutState(sync);
  if (c->frame > 1000)
    MaxonAction(motor);


  s_->x = EncoderGetAngle(encoder);
  s_->motor = MaxonGetState(motor);
  s_->dx = haptic->dtheta;
  s_->tau_active = haptic->tau_active;
}

void Pru0Cleanup(void) {
 // MaxonMotorFree(motor);
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

  mem->p->P0 = P0;
  mem->p->threshold = reflexthreshold;
  mem->p->dP = reflexdelta;

  dt = fix16_sdiv(fix16_one,fix16_from_int(mem->p->fs_hz));

  i2c1 = I2cInit(1);
  mux = MuxI2cInit(i2c1,0x70,PCA9548);
  reservoir = PamReservoirInit(PressureSensorInit(mux,0,0x28));

  pam1 = PamInitMuscle(PressureSensorInit(mux,1,0x28), 0, 1, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,mem->p->P0);

  pam2 = PamInitMuscle(PressureSensorInit(mux,2,0x28), 2, 3, refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,mem->p->P0);

  reflex = ReflexInit(pam1,pam2,fix16_from_int(15), fix16_from_int(95),
                      FiltIirInit(1, b_dcblck, a_dcblck));

  reflex = ReflexMyoInit(pam1,pam2,emg1,emg2,
                      fix16_from_int(15), fix16_from_int(95),
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
  //ReflexMyoUpdate(reflex,
  //                s_->emg1_state.bits,
//                s_->emg2_state.bits,
//                0x320000, p_->dP);
//
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  tt += dt;
  fix16_t ref = fix16_smul(0x3333,
      fix16_sin(fix16_smul(fix16_smul(0x199A,
            fix16_smul(0x20000,fix16_pi)),tt)));

  ReflexUpdate(reflex, p_->threshold, p_->dP, ref);


  if (PruGetCtlBit(ctl_,2)) {
    PamSetPd(pam1,p_->P0);
    PamSetPd(pam2,p_->P0);
    PruClearCtlBit(ctl_,2);
  }


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

