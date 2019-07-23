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

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// haptic interface
const uint32_t Td_default = 5000;
const uint32_t Np_default = 1;
uint32_t flag, t0, t;

// Pam stuff
i2cmux_t* mux;

reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;

const uint8_t mux_i2c_add = 0x70;
const uint8_t sensor_i2c_add = 0x28;
const uint8_t res_chan = 4;

const uint8_t pam_pro_ch = 6;
const uint8_t pam_pro_hp_pin = 5;
const uint8_t pam_pro_lp_pin = 4;
const uint8_t pam_sup_ch = 3;
const uint8_t pam_sup_hp_pin = 7;
const uint8_t pam_sup_lp_pin = 6;

const fix16_t p_default = 0xA0000; // 10 psi

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {

  // init params
  mem->p->Td = Td_default;
  mem->p->Np = Np_default;
  mem->p->pd = p_default;

  flag = 0;
  encoderInit();
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  if (PruGetCtlBit(ctl_, 0)) {
    if (flag == 0) {
      t0 = c->frame;
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
    }
	} else {
    s_->xd = 0;
  }
  encoderSample(&s_->x);
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
  mux = MuxI2cInit(mux_i2c_add,PCA9548);

  reservoir = PamReservoirInit(
                PressureSensorInit(mux,res_chan,sensor_i2c_add));

  pam1 = PamInitMuscle(PressureSensorInit(mux,pam_pro_ch,sensor_i2c_add),
                        pam_pro_hp_pin, pam_pro_lp_pin);
  PamSetPd(pam1,fix16_from_int(10));

  pam2 = PamInitMuscle(PressureSensorInit(mux,pam_sup_ch,sensor_i2c_add),
                        pam_sup_hp_pin, pam_sup_lp_pin);
  PamSetPd(pam2,fix16_from_int(10));

}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  // update pd if param changed
  if (PruGetCtlBit(ctl_, 1)) {
    PamSetPd(pam1,p_->pd);
    PamSetPd(pam2,p_->pd);
    PruClearCtlBit(ctl_,1);
  }

  PamActionSimple(pam1);
  PamActionSimple(pam2);

  s_->pam1_state = PamGetState(pam1);
  s_->pam2_state = PamGetState(pam2);



}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
  PamReservoirUpdate(reservoir);
  s_->p_res = PamReservoirGetPressure(reservoir);

}

void Pru1Cleanup(void) {
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
  PamReservoirFree(reservoir);
  MuxI2cFree(mux);
}

