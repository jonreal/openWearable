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
#include "filtcoeff.h"
#include "hx711.h"
#include "sync.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pam
i2cmux_t* mux;
i2c_t* i2c;
pam_t* pam;
reservoir_t* reservoir;

// Loadcells
hx711_t* loadcell_r;
hx711_t* loadcell_l;

// Sync In
sync_t* sync;

uint32_t flag = 0;
uint32_t cyclecnt = 0;

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

  i2c = I2cInit(2);
  mux = MuxI2cInit(i2c,0x70,PCA9548);

  // Ch. 8, 8-1 = 7
  reservoir = PamReservoirInit(PressureSensorInit(mux,7,0x28));

  // pam
  // sensor on mux ch. 6, 6 - 1 = 5
  // in: P8.45, MODE5, pr1_pru1_pru_r30_0
  // out: 8.46, MODE5, pr1_pru1_pru_r30_1
  pam = PamInitMuscle(PressureSensorInit(mux,5,0x28),
                        reservoir,
                        0, 1,
                        0x1F8A034, // 504.6258 ms
                        0x2FAAD48, // 762.6769 ms
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam,0);

  // Load Cell r (right support rail, wrt to participant)
  // CLK: pr1_pru1_pru_r30_3, P8.44
  // DOUT: pr1_pru1_pru_r31_6, P8.39
  loadcell_r = Hx711InitLoadCell(3, 6, HX711_GAIN_64);

  // Load Cell l (left support rail, wrt to participant)
  // CLK: P8.43, MODE5, pr1_pru1_pru_r30_2
  // DOUT: P8.40, MODE6, pr1_pru1_pru_r31_7
  loadcell_l = Hx711InitLoadCell(2, 7, HX711_GAIN_64);

  // Sync In
  // In: P8.42, MODE6, pr1_pru1_pru_r31_5 */
  sync = SyncInitChan(5);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  if (PruGetCtlBit(ctl_, 0)) {
    if (cyclecnt < p_->Tdelay) {                    // PRE
      s_->status = 0;
    } else if (cyclecnt < p_->Ttrial) {             // ACTIVE
      PamSetPd(pam, p_->Ptarget);
      s_->status = 1;
    } else {                                        // POST
      PamSetPd(pam, 0);
      PruClearCtlBit(ctl_, 0);
      s_->status = 0;
      cyclecnt = 0;
      return;                                       // no extra +1 tick
    }
    cyclecnt++;
  } else {
    s_->status = 0;
    cyclecnt = 0;
  }



  PamReservoirUpdate(reservoir);
  s_->p_res = PamReservoirGetPressure(reservoir);

  PamActionSimple(pam);
  PamUpdate(pam);
  s_->pam_state = PamGetState(pam);

  s_->load_r = Hx711Sample(loadcell_r);
  s_->load_l = Hx711Sample(loadcell_l);

  SyncInUpdate(sync);
  s_->sync_state = SyncGetState(sync);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam);
  Hx711Free(loadcell_r);
  Hx711Free(loadcell_l);
}

