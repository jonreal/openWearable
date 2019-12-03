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

#include "spidriver.h"


volatile register uint32_t __R30;
volatile register uint32_t __R31;


// Haptic pru0
sync_t* sync;
motor_t* motor;
encoder_t* encoder;
hapitic_t* haptic;

fix16_t t = 0;
fix16_t dt = 0;
uint32_t flag = 0;
uint32_t itarg = 0;
uint32_t targtimeout = 5000;
uint32_t timeoutcnt = 0;
uint32_t debounce = 0;
fix16_t theta0 = 0;


// PAMs pru1
i2c_t* i2c1;
i2cmux_t* mux;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
reflex_t* reflex;
reflex_myo_t* reflexmyo;
emg_t* emg1;
emg_t* emg2;

// EMGs are downsampled
// butter(1,200/250,'high')
const fix16_t k_hp_emg_b[2] = {0x3EC5,-0x3EC5};
const fix16_t k_hp_emg_a[2] = {fix16_one,0x826F};

// butter(1,3/250)
const fix16_t k_lp_emg_b[2] = {0x4BC,0x4BC};
const fix16_t k_lp_emg_a[2] = {fix16_one,-0xF687};

// DC blocking filter
const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF3333};  // -0.80

const uint32_t refractory = 150;


// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {

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
                         0x4173290    // max velocity (10000 rpm ~1047 rad/s)
                         );
  haptic = HapticInit(motor,encoder,
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a),
                      0x1F6A7A // 10pi (5 Hz)
                      );

  emg1 = EmgInitSensor(0x0,
                    FiltIirInit(1, k_hp_emg_b, k_hp_emg_a),
                    FiltIirInit(1, k_lp_emg_b, k_lp_emg_a));
  emg2 = EmgInitSensor(0x1,
                    FiltIirInit(1, k_hp_emg_b, k_hp_emg_a),
                    FiltIirInit(1, k_lp_emg_b, k_lp_emg_a));

  dt = fix16_sdiv(fix16_one,fix16_from_int(mem->p->fs_hz));

}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_){
  EncoderUpdate(encoder);
  MaxonUpdate(motor);
  HapticPendulumUpdate(haptic,
      p_->Jvirtual, p_->bvirtual, p_->kvirtual, theta0, 0, p_->G);

  // must down sample adc
  if (c->frame % 2) {
    EmgUpdate(emg1);
  } else {
    EmgUpdate(emg2);
  }
  s_->emg1_state = EmgGetState(emg1);
  s_->emg2_state = EmgGetState(emg2);
  s_->x = EncoderGetAngle(encoder);
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){

  // Tracking
  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      SyncOutHigh(sync);
      flag = 1;
      t = 0;
    }
    s_->game = 1;
		if (t >= p_->Td){
      flag = 0;
      s_->game = 0;
      PruClearCtlBit(ctl_,0);
      SyncOutLow(sync);
    }
    s_->xd = fix16_smul(0x5A0000, fix16_sin(fix16_smul(p_->k2PiFd,t)));
	}
  // Ballistic
  else if (PruGetCtlBit(ctl_, 1)) {
    if (flag == 0) {
      SyncOutHigh(sync);
      flag = 1;
      t = 0;
    }
		if (itarg == p_->Nb) {
      flag = 0;
      itarg = 0;
      PruClearCtlBit(ctl_,1);
      SyncOutLow(sync);
      s_->game = 0;
    }
    s_->game = 2;
    s_->xd = p_->targets[itarg];
    timeoutcnt++;
    if ((fix16_ssub(fix16_ssub(s_->xd,s_->x),0x40000) < 0)
       && (fix16_sadd(fix16_ssub(s_->xd,s_->x),0x40000) > 0)
       || (timeoutcnt > targtimeout)){
      debounce++;
      if ((debounce > 250) || (timeoutcnt > targtimeout)) {
        itarg++;
        s_->xd = p_->targets[itarg];
        debounce = 0;
        timeoutcnt = 0;
      }
    }
    theta0 = s_->xd;
  } else {
    s_->xd = 0;
    theta0 = 0;
    s_->game = 0;
  }

  s_->vsync = (uint32_t)SyncOutState(sync);
  if (c->frame > 1000)
    MaxonAction(motor);
  s_->motor = MaxonGetState(motor);
  s_->dx = haptic->dtheta;
  s_->tau_active = haptic->tau_active;
  t += dt;
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

  reflex = ReflexInit(pam1,pam2,fix16_from_int(15), fix16_from_int(95),
                      FiltIirInit(1, b_dcblck, a_dcblck));

  reflexmyo = ReflexMyoInit(pam1,pam2,emg1,emg2,
                      fix16_from_int(15), fix16_from_int(95));
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

  switch (p_->reflex_condition) {
    case 0:
      break;
    case 1:
      ReflexMyoUpdate(reflexmyo,
                      s_->emg1_state.value,
                      s_->emg2_state.value,
                      p_->emg_up_thres, p_->emg_low_thres, p_->dP);
      break;
    case 2:
      ReflexUpdate(reflex, p_->threshold, p_->dP, 0);
      break;
    case 3:
      ReflexMyoUpdate(reflexmyo,
                      s_->emg1_state.value,
                      s_->emg2_state.value,
                      p_->emg_up_thres, p_->emg_low_thres, p_->dP);
      ReflexUpdate(reflex, p_->threshold, p_->dP, 0);
      break;
  }


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
  s_->triggersignal_myo = reflexmyo->triggersignal;
}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

