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
#include "reflex.h"
#include "potentiometer.h"
#include "epcontroller.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pams
i2cmux_t* mux;
i2c_t* i2c;
reservoir_t* reservoir;
pam_t* pam1;
pam_t* pam2;
reflex_t* reflex;
potentiometer_t* pot1;
potentiometer_t* pot2;
epcontroller_t* ep;

// Not connected, only sensor
//pam_t* pam3;
//pam_t* pam4;

// DC blocking filter
const fix16_t b_dcblck[2] = {fix16_one, -fix16_one};
const fix16_t a_dcblck[2] = {fix16_one, 0xFFFF3333};  // -0.80

const uint32_t refractory = 150;

// EP re-commands its baseline only when it moves by more than this (fix16 ~0.05),
// so a sustained reflex jump isn't pulled back each tick (both edit pam->pd).
const fix16_t EP_APPLY_EPS = 3277;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------

void Pru0Init(pru_mem_t* mem) {

  // pot1 - adc ch 0 (EP), pot2 - adc ch 1 (stiffness). The pot filters its own
  // noise (1.3 Hz LP) so userspace hands the EP controller a clean signal.
  pot1 = PotentiometerInit(0, FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  pot2 = PotentiometerInit(1, FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));

}

void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {

  PotentiometerUpdate(pot1);
  PotentiometerUpdate(pot2);

  io->s->pot1 = pot1->value;
  io->s->pot2 = pot2->value;

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

  // i2c 2
  // SDA - p9.20
  // SCL - p9.19
  i2c = I2cInit(2);
  mux = MuxI2cInit(i2c,0x70,PCA9548);

  //// Ch. 7, 7-1 = 6. i2c round-robin: decimate=3, staggered phases 0/1/2
  //// (reservoir/pam1/pam2) -> one pressure read per tick.
  reservoir = PamReservoirInit(PressureSensorInit(mux,6,0x28,3,0));


  // pam1
  // sensor on mux ch. 6, 6 - 1 = 5
  // out: P8.28, pr1_pru1_pru_r30_10
  // in: 8.27, pr1_pru1_pru_r30_8
  pam1 = PamInitMuscle(PressureSensorInit(mux,5,0x28,3,1),
                        reservoir,
                        8, 10,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));

  // pam2
  // sensor on mux ch. 5, 5 - 1 = 4
  // out: P8.30, pr1_pru1_pru_r30_9
  // in: 8.29, pr1_pru1_pru_r30_11
  pam2 = PamInitMuscle(PressureSensorInit(mux,4,0x28,3,2),
                        reservoir,
                        9, 11,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));




  // Reflex (unchanged): owns the PAM pair, jumps the EP on a load trigger.
  reflex = ReflexInit(pam1, pam2, fix16_from_int(5), fix16_from_int(95),
                      FiltIirInit(1, b_dcblck, a_dcblck));

  // EP / impedance controller: pots -> zero-load antagonist setpoints. Pure law
  // (no filter -- the pots deliver clean signals); Pmax set live from params.
  ep = EpControllerInit(0);
  //// pam3
  //// sensor on mux ch. 4, 4 - 1 = 3
  //// out: NC
  //// in: NC
  //pam3 = PamInitMuscle(PressureSensorInit(mux,3,0x28),
  //                      reservoir,
  //                      4, 5,
  //                      refractory,
  //                      FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  //PamSetPd(pam3,0);

  //// pam4
  //// sensor on mux ch. 3, 3 - 1 = 2
  //// out: NC
  //// in: NC
  //pam4 = PamInitMuscle(PressureSensorInit(mux,2,0x28),
  //                      reservoir,
  //                      6, 7,
  //                      refractory,
  //                      FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  //PamSetPd(pam4,0);

}

void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {

  PamReservoirUpdate(reservoir);

  PamUpdate(pam1);
  PamUpdate(pam2);


  //PamUpdate(pam3);
  //PamUpdate(pam4);

}

void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {

  // --- EP controller: pots -> filtered zero-load antagonist baseline (Pd1/Pd2).
  //     pot1 -> EP theta_d in [-1,1] (2*frac - 1); pot2 -> stiffness k in [0,1].
  fix16_t potA = fix16_div(fix16_from_int((int) io->s->pot1), fix16_from_int(4095));
  fix16_t potB = fix16_div(fix16_from_int((int) io->s->pot2), fix16_from_int(4095));
  EpControllerSetPmax(ep, view->p->Pmax);
  EpControllerSetEp(ep, fix16_ssub(fix16_smul(fix16_from_int(2), potA), fix16_one));
  EpControllerSetK(ep, potB);
  EpControllerUpdate(ep);

  // EP is an event-driven editor: (re)command the baseline only when it actually
  // moves, so a reflex jump isn't pulled back every tick. Both edit pam->pd; the
  // PamSetPd HOLD-gate serializes them (no mid-move stomping).
  fix16_t pd1 = EpControllerGetPd1(ep);
  fix16_t pd2 = EpControllerGetPd2(ep);
  static fix16_t last_pd1 = 0, last_pd2 = 0;
  static uint32_t primed = 0;
  fix16_t d1 = fix16_ssub(pd1, last_pd1); if (d1 < 0) d1 = -d1;
  fix16_t d2 = fix16_ssub(pd2, last_pd2); if (d2 < 0) d2 = -d2;
  if (!primed || d1 > EP_APPLY_EPS || d2 > EP_APPLY_EPS) {
    PamSetPd(pam1, pd1);
    PamSetPd(pam2, pd2);
    last_pd1 = pd1; last_pd2 = pd2; primed = 1;
  }

  // Reflex (unchanged, owns the PAMs): jumps the EP under load, when enabled.
  if (PruCmd(io, CMD_REFLEX))
    ReflexUpdate(reflex, view->p->threshold, view->p->dP, 0);

  PamActionSimple(pam1);
  PamActionSimple(pam2);

  io->s->p_res = PamReservoirGetPressure(reservoir);
  io->s->pam1_state = PamGetState(pam1);
  io->s->pam2_state = PamGetState(pam2);



  //s_->pam3_state = PamGetState(pam3);
  //s_->pam4_state = PamGetState(pam4);
  io->s->triggersignal = reflex->triggersignal;

  // Echo command state into telemetry so the operator console confirms what the
  // board is actually running (params + reflex + arm), no command acks needed.
  io->s->pmax      = view->p->Pmax;
  io->s->dp        = view->p->dP;
  io->s->thr       = view->p->threshold;
  io->s->reflex_on = PruCmd(io, CMD_REFLEX) ? 1u : 0u;
  io->s->armed     = (*io->cmd & ARM_ARMED) ? 1u : 0u;

}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

