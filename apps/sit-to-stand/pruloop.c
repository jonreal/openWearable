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

void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {
}

void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io) {
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

  // Ch. 6, 6-1 = 5
  reservoir = PamReservoirInit(PressureSensorInit(mux,5,0x28));

  // pam
  // sensor on mux ch. 6, 6 - 1 = 5
  // IN: * P8.46, MODE5, pr1_pru1_pru_r30_1 */
  // OUT:* P8.44, MODE5, pr1_pru1_pru_r30_3 */

  pam = PamInitMuscle(PressureSensorInit(mux,5,0x28),
                        reservoir,
                        1, 3,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam,0);

  //// Load Cell r (right support rail, wrt to participant)
  //// CLK: pr1_pru1_pru_r30_3, P8.44
  //// DOUT: pr1_pru1_pru_r31_6, P8.39
  //loadcell_r = Hx711InitLoadCell(3, 6, HX711_GAIN_64);

  //// Load Cell l (left support rail, wrt to participant)
  //// CLK: P8.43, MODE5, pr1_pru1_pru_r30_2
  //// DOUT: P8.40, MODE6, pr1_pru1_pru_r31_7
  //loadcell_l = Hx711InitLoadCell(2, 7, HX711_GAIN_64);

  // Sync In
  ///* P8.45, MODE5, pr1_pru1_pru_r30_0 */
  sync = SyncInitChan(0);
}

void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {

  // Trial phases (Option A): PRE(Tdelay) -> UP(Ttrial, inflated) -> DOWN(Ttrial,
  // deflated) -> END. UP captures the sit-to-stand transition; DOWN keeps the log
  // running through deflation so the stand-to-sit transition is captured too. Only
  // END returns early (clears the ctl bit -> uiloop stops the log); PRE/UP/DOWN all
  // fall through to the sensor reads below. status: 0=baseline/idle, 1=up, 2=down.
  if (PruGetCtlBit(io->ctl, 0)) {
    if (cyclecnt < view->p->Tdelay) {                             // PRE (baseline, deflated)
      io->s->status = 0;
      SyncOutHigh(sync);
    } else if (cyclecnt < view->p->Tdelay + view->p->Ttrial) {   // UP (inflate -> sit-to-stand)
      PamSetPd(pam, view->p->Ptarget);
      io->s->status = 1;
    } else if (cyclecnt < view->p->Tdelay + 2*view->p->Ttrial) { // DOWN (deflate -> stand-to-sit)
      PamSetPd(pam, 0);
      io->s->status = 2;
    } else {                                                     // END
      PamSetPd(pam, 0);
      SyncOutLow(sync);
      PruClearCtlBit(io->ctl, 0);
      io->s->status = 0;
      cyclecnt = 0;
      return;                                                    // no extra +1 tick
    }
    cyclecnt++;
  } else {
    io->s->status = 0;
    cyclecnt = 0;
  }



  PamReservoirUpdate(reservoir);
  io->s->p_res = PamReservoirGetPressure(reservoir);

  PamActionSimple(pam);
  PamUpdate(pam);
  io->s->pam_state = PamGetState(pam);

  //io->s->load_r = Hx711Sample(loadcell_r);
  //io->s->load_l = Hx711Sample(loadcell_l);

  SyncInUpdate(sync);
  io->s->sync_state = SyncGetState(sync);
}

void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {
}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam);
  //Hx711Free(loadcell_r);
  //Hx711Free(loadcell_l);
}
