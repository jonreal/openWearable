/* Copyright 2018 Jonathan Realmuto

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


#include "pam.h"
#include <stdlib.h>

volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

// ---- static functions -----------------------------------------------------
static void PamUpdateControl(pam_t* pam) {
  switch (pam->s.u) {
    case 0:
      __R30 &= ~(1 << pam->hp_pin) & ~(1 << pam->lp_pin);
      break;
    case 1:
      __R30 |= (1 << pam->hp_pin);
      __R30 &= ~(1 << pam->lp_pin);
      break;
    case -1:
      __R30 |= (1 << pam->lp_pin);
      __R30 &= ~(1 << pam->hp_pin);
      break;
  }
}


// ---------------------------------------------------------------------------
reservoir_t* PamReservoirInit(pressure_sensor_t* sensor) {
  reservoir_t* reservoir = malloc(sizeof(reservoir_t));
  reservoir->sensor = sensor;
  PamReservoirUpdate(reservoir);
  return reservoir;
}

void PamReservoirFree(reservoir_t* reservoir) {
  PressureSensorFree(reservoir->sensor);
  free(reservoir);
}

void PamReservoirUpdate(reservoir_t* reservoir){
  reservoir->pressure = PressureSensorSample(reservoir->sensor);
}

fix16_t PamReservoirGetPressure(const reservoir_t* reservoir) {
  return reservoir->pressure;
}


// ---------------------------------------------------------------------------
pam_t* PamInitMuscle(pressure_sensor_t* sensor,
                     uint32_t in_pin,
                     uint32_t out_pin) {
  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = sensor;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->flag = 0;
  pam->s.u = 0;
  pam->s.p_m = 0;
  pam->s.p_d = 0;

  // Will hang here if i2c mux/channel is off
  PamUpdate(pam);
  return pam;
}



void PamMuscleFree(pam_t* pam) {
   __R30 |= (1 << pam->lp_pin);
   __R30 &= ~(1 << pam->hp_pin);
  PressureSensorFree(pam->sensor);
  free(pam);
}

pam_state_t PamGetState(const pam_t* pam) {
  return pam->s;
}

void PamUpdate(pam_t* pam) {
  pam->s.p_m = PressureSensorSample(pam->sensor);
}

void PamSetU(pam_t* pam, int8_t u) {
  pam->s.u = u;
}

void PamSetPd(pam_t* pam, fix16_t Pd) {
  pam->flag = 0;
  pam->s.p_d = Pd;
}

void PamActionSimple(pam_t* p) {

  // update sensors
  PamUpdate(p);

  // reached goal flag
  if (fix16_ssub(p->s.p_d,fix16_ssub(p->s.p_m,0x50000)) < 0)
    p->flag = 1;

  // if pd = 0 open exhaust
  if (p->s.p_d == 0)
    PamSetU(p,-1);
  else if ((fix16_ssub(p->s.p_d,fix16_ssub(p->s.p_m,0x50000)) > 0) && (p->flag==0))
    PamSetU(p,1);
  else
    PamSetU(p,0);

  PamUpdateControl(p);
}




