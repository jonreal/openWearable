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

//
// Include sensor/actuator/algorithmic modules here
//
#include "pam.h"
#include "filter.h"
#include "filtcoeff.h"
#include "emg.h"
#include "halleffect.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// external sensor
hall_t* encoder;
iir_filt_t* lp_1_5Hz;

//nlb_filt_t* nlb;

// PAMs
pam_t* pam1;
pam_t* pam2;
const uint8_t div = 1;
const fix16_t threshold = fix16_one;
const fix16_t p_min = 0xF0000; // 15 psi
const uint8_t sensor_add = 0x28;
const uint8_t bus_add = 0x77;

// EMG
emg_t* emg_pro;
emg_t* emg_sup;
//

//
// emg constants
const fix16_t k_emg_bias = 0x37A0000; // 890 bits
const fix16_t k_emg_scaling = 0x206;  // 4/507  (scaling/normalizing const)

//// NLB constants
//const fix16_t k_alpha = 0x42;     // 0.001
//const fix16_t k_beta = 0x28F;     // 0.01
//

// Prototypes
fix16_t conditionEmg(fix16_t in);

uint32_t t0, t;
uint8_t flag = 0;
uint32_t P = 5000;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {

  // Sensors
  emg_pro = EmgInitSensor(1);
  emg_sup = EmgInitSensor(2);

//  nlb = FiltNlbInit(4,
//                    fix16_from_int(1000),
//                    fix16_one,
//                    k_alpha,
//                    k_beta);

  // External Sensors
  encoder = HallInitSensor(0,fix16_from_int(180));
  lp_1_5Hz = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  // EMG
  s_->emg1_raw = EmgSample(emg_pro);
  s_->emg1_rect = conditionEmg(s_->emg1_raw);
  //s_->emg_nlb = FiltNlb(s_->emg_rect, nlb);
  s_->emg2_raw = EmgSample(emg_sup);
  s_->emg2_rect = conditionEmg(s_->emg2_raw);
//s_->emg_nlb = FiltNlb(s_->emg_rect, nlb);

  // External Sensors
  s_->angle = FiltIir(HallGetAngleDeg(encoder),lp_1_5Hz);
  //s_->angle = HallGetAngleDeg(encoder);

  if (ctl_->bit.utility & (1 << 0)) {
    if (flag == 0) {
      t0 = c->frame;
      flag = 1;
    }
    t = (c->frame - t0)*1000 / (P - P/1000);

    // 90.0*(sin(2*pi*t) + 1) - 90;
    s_->angle_d = fix16_ssub(fix16_smul(fix16_from_int(90),
                    fix16_sadd(
                      fix16_sdiv(
                        fix16_from_int( (int32_t) l_->lut[t % 1000]),
                      fix16_from_int(1000)),
                    fix16_one)),fix16_from_int(90));

    if (t == 10000) {
      flag = 0;
      ctl_->bit.utility = 0;
    }
  } else {
    s_->angle_d = 0;
  }
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
}

void Pru0Cleanup(void) {
  EmgCleanup(emg_pro);
  EmgCleanup(emg_sup);
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

//  pam1 = PamInitMuscle(0x77, 0x3, 0x28, 10, 8, div, threshold);
//  pam2 = PamInitMuscle(0x70, 0x1, 0x28, 10, 8, div, threshold);

//  pam1 = PamInitMuscle(SENSOR_ADD, MUX_SEL, 0, EXT_V_HP, EXT_V_LP, div,
//                      threshold);
//  pam2 = PamInitMuscle(SENSOR_ADD, MUX_SEL, 1, EXT_V_HP, EXT_V_LP, div,
//                      threshold);
//
//
//  lp_1_5Hz_1 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);
//  lp_1_5Hz_2 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);
//
//  mem->s->state->p_d = p_min;
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

//  s_->p1_m = PamSamplePressure(pam1);
//  s_->p2_m = PamSamplePressure(pam2);
 //   debug_buff[2] = c->frame;

//  PamSamplePressure(pam1, &s_->p1_m);
//  PamSamplePressure(pam2, &s_->p2_m);
//  s_->angle = s_->p_m;
//  s_->p_m = FiltIir(s_->p_m,lp_1_5Hz_1);
//  s_->dp_m = s_->p_m - FiltIir(s_->p_m,lp_1_5Hz_2);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

 // s_->p_d = s_->p_d - fix16_smul(K2,s_->dp_m);

 // if (s_->p_d < p_min)
 //   s_->p_d = p_min;
 //
 // s_->p_d = p_min;
 // PamUpdateControl(pam, &s_->p_m, &s_->p_d, &s_->u);
}

void Pru1Cleanup(void) {
//  PamFreeMuscle(pam1);
//  PamFreeMuscle(pam2);
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
fix16_t conditionEmg(fix16_t in) {
  fix16_t out = fix16_ssub(in, k_emg_bias);
  // rectify
  if (out < 0)
   out = -out;
  return fix16_smul(k_emg_scaling, out);
}
