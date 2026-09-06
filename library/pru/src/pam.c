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
#include "error.h"
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
pam_t* PamInitMuscle(pressure_sensor_t* sens,
                      reservoir_t* reservoir,
                      uint32_t in_pin,
                      uint32_t out_pin,
                      uint32_t refract,
                      iir_filt_t* filter){
  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = sens;
  pam->res = reservoir;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->T_refract = refract;
  pam->fsm = HOLD;
  pam->s.u = 0;
  pam->s.pm = 0;
  pam->s.pd = 0;
  pam->filt = filter;
  pam->cnt = 0;
  pam->drop_count = 0;

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
  i2c_t* i2c = pam->sensor->mux->i2c;   // i2c fault side-channel
  i2c->last_err = ERR_NONE;             // clear-before

  fix16_t sample = PressureSensorSample(pam->sensor);

  if (pam->sensor->fresh) {             // only judge faults on a real (non-decimated) read
    if (i2c->last_err != ERR_NONE) {    // check-after: this read dropped
      if (++pam->drop_count >= error_max_consecutive_drops)
        ErrorRaise((error_code_t) i2c->last_err,      // core-agnostic (see ErrorInit)
                   ((uint32_t) pam->sensor->mux_channel << 8)
                     | pam->sensor->i2c_address);
      return;                           // sample-hold: keep last pm
    }
    pam->drop_count = 0;                // good read: reset the run
  }

  // Fresh read or decimated skip: filter at full rate on the (fresh or held) sample.
  pam->s.pm_raw = sample;
  if (pam->filt)
    pam->s.pm = FiltIir(pam->s.pm_raw,pam->filt);
  else
    pam->s.pm = pam->s.pm_raw;
}

void PamSetU(pam_t* pam, int8_t u) {
  pam->s.u = u;
}

void PamSetPd(pam_t* pam, fix16_t Pd) {

  if (Pd == 0)
    pam->fsm = ZERO;

  if ((pam->fsm == HOLD) || (pam->fsm == ZERO)){
    if (Pd == 0) {
      pam->fsm = ZERO;
    } else if (Pd < pam->s.pm) {
      pam->fsm = DEFLATE;
      pam->s.pd = Pd;
    } else if (Pd > pam->s.pm) {
      pam->fsm = INFLATE;
      pam->s.pd = Pd;
    }
  }
}

void PamActionSimple(pam_t* p) {
  switch (p->fsm) {
    case ZERO : {
      p->fsm = ZERO;
      PamSetU(p,-1);
      break;
    }
    case INFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) < 0) {
        p->fsm = REFRACT;
        PamSetU(p,0);
      } else {
          PamSetU(p,1);
      }
      break;
    }
    case DEFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) > 0) {
        p->fsm = REFRACT;
        PamSetU(p,0);
      } else {
        PamSetU(p,-1);
      }
      break;
    }
    case REFRACT : {
      if (p->cnt == p->T_refract) {
        p->fsm = HOLD;
        p->cnt = 0;
      } else {
        p->cnt++;
      }
      break;
    }
  }
  PamUpdateControl(p);
}
