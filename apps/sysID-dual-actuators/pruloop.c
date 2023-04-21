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
#include "pam.h"
#include "maxon.h"
#include "encoder.h"
#include "sync.h"
#include "filtcoeff.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

motor_t* motor;
encoder_t* encoder;
sync_t* sync;

i2cmux_t* mux;
i2c_t* i2c1;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;

fix16_t pd1 = 0;
fix16_t pd2 = 0;
uint32_t flag = 0;
uint32_t t0 = 0;
fix16_t t = 0;
fix16_t dt = 0;
fix16_t k = 0;
fix16_t f = 0;
fix16_t df = 0;
// fix16_t step = 0;
fix16_t Ton = 30000;
fix16_t Toff = 5000;
const uint32_t refractory = 4000;
uint32_t pulsecnt = 0;
fix16_t count = 0;


// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------

void Pru0Init(pru_mem_t* mem) {

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
//  EncoderTare(encoder);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  EncoderUpdate(encoder);
  MaxonUpdate(motor);

  if (PruGetCtlBit(ctl_, 0)) {
    count += dt;

    if (flag == 0) {
      t0 = c->frame;
      f = p_->f0;
//      cycle = p_->cycle;
//      df = fix16_sdiv(fix16_ssub(p_->f1,p_->f0),
//           fix16_ssub(p_->cycle,0x10000));
      df = p_->step;
      SyncOutHigh(sync);
      flag = 1;
    }

    if (count >= 0x50000){
      t += dt;
      if (t >= fix16_sdiv(fix16_smul(p_->repeat,0x10000),f)){
        f += df;
        t = 0;
      }

      MaxonSetCurrent(motor,fix16_smul(p_->A,fix16_sin(fix16_sadd(fix16_smul(
                      fix16_smul(fix16_smul(0x20000,fix16_pi),t),f),
                      fix16_sdiv(fix16_pi,0x20000)))));
/*       MaxonSetCurrent(motor,fix16_sadd(fix16_smul(p_->A,fix16_sin(fix16_sadd(
                       fix16_smul(fix16_smul(fix16_smul(0x20000,fix16_pi),t),f),
                       fix16_sdiv(fix16_pi,0x20000)))),-0x0B43));*/

  	  if (f > p_->f1) {
        MaxonSetCurrent(motor,0);
        SyncOutLow(sync);
        PruClearCtlBit(ctl_,0);
      }
    }
  }else{
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
}


void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamReservoirUpdate(reservoir);

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  if (PruGetCtlBit(ctl_, 0)) {
    if (pulsecnt <= 40000) {
      pd1 = p_->Pd1;
      pd2 = p_->Pd2;
      PamSetPd(pam1,pd1);
      PamSetPd(pam2,pd2);
    }
    pulsecnt++;
   }

  PamActionSimpleXWL(pam1);
  PamActionSimpleXWR(pam2);
  PamUpdate(pam1);
  PamUpdate(pam2);
  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);
  s_->p_res = PamReservoirGetPressure(reservoir);
}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
 PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

