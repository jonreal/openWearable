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

// Not connected, only sensor
//pam_t* pam3;
//pam_t* pam4;

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

  // pot1 - adc ch 0
  // pot2 - adc ch 1
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

  //// Ch. 7, 7-1 = 6
  reservoir = PamReservoirInit(PressureSensorInit(mux,6,0x28));


  // pam1
  // sensor on mux ch. 6, 6 - 1 = 5
  // out: P8.28, pr1_pru1_pru_r30_10
  // in: 8.27, pr1_pru1_pru_r30_8
  pam1 = PamInitMuscle(PressureSensorInit(mux,5,0x28),
                        reservoir,
                        8, 10,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam1,fix16_from_int(20));

  // pam2
  // sensor on mux ch. 5, 5 - 1 = 4
  // out: P8.30, pr1_pru1_pru_r30_9
  // in: 8.29, pr1_pru1_pru_r30_11
  pam2 = PamInitMuscle(PressureSensorInit(mux,4,0x28),
                        reservoir,
                        9, 11,
                        refractory,
                        FiltIirInit(1, k_lp_1_3Hz_b, k_lp_1_3Hz_a));
  PamSetPd(pam2,fix16_from_int(20));

  // i2c round-robin: one pressure read per tick (reservoir/pam1/pam2 staggered).
  PressureSensorSetDecimate(reservoir->sensor, 3, 0);
  PressureSensorSetDecimate(pam1->sensor,      3, 1);
  PressureSensorSetDecimate(pam2->sensor,      3, 2);




//
//  reflex = ReflexInit(pam1,pam2,fix16_from_int(5), fix16_from_int(95),
//                      FiltIirInit(1, b_dcblck, a_dcblck));
//
//
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

//  ReflexUpdate(reflex, p_->threshold, p_->dP, 0);
//
//  if (PruGetCtlBit(ctl_,2)) {
//    PamSetPd(pam1,p_->P0);
//    PamSetPd(pam2,p_->P0);
//    PruClearCtlBit(ctl_,2);
//  }
//
  PamActionSimple(pam1);
  PamActionSimple(pam2);

  io->s->p_res = PamReservoirGetPressure(reservoir);
  io->s->pam1_state = PamGetState(pam1);
  io->s->pam2_state = PamGetState(pam2);



  //s_->pam3_state = PamGetState(pam3);
  //s_->pam4_state = PamGetState(pam4);
  io->s->triggersignal = reflex->triggersignal;

}

void Pru1Cleanup(void) {
  PamReservoirFree(reservoir);
  PamMuscleFree(pam1);
  PamMuscleFree(pam2);
}

