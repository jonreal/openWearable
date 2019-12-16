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
pam_t* PamInitMuscle(pressure_sensor_t* sens,
                      reservoir_t* reservoir,
                      uint32_t in_pin,
                      uint32_t out_pin,
                      fix16_t tau_in,
                      fix16_t tau_out,
                      uint32_t refract,
                      iir_filt_t* filter){
  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = sens;
  pam->res = reservoir;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->tau_in = tau_in;
  pam->tau_out = tau_out;
  pam->T_refract = refract;
  pam->fsm = HOLD;
  pam->s.u = 0;
  pam->s.pm = 0;
  pam->s.pd = 0;
  pam->filt = filter;
  pam->pcnt = 0;
  pam->pw = 0;

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
  pam->s.pm_raw = PressureSensorSample(pam->sensor);
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
      //pam->pw = (uint32_t)
      //  (-fix16_mul(pam->tau_out,fix16_log(fix16_div(Pd,pam->s.pm))));
    } else if (Pd > pam->s.pm) {
      pam->fsm = INFLATE;
      pam->s.pd = Pd;
      //pam->pw = (uint32_t)
      //  (-fix16_mul(pam->tau_out,
      //    fix16_log(fix16_div(fix16_sub(Pd,pam->res->pressure),
      //              fix16_sub(pam->s.pm,pam->res->pressure)))));
    }
  }
}

void PamActionSimple(pam_t* p) {
  switch (p->fsm) {
    case ZERO : {
      p->fsm = ZERO;
      PamSetU(p,-1);
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

void PamActionModel(pam_t* p) {

  switch (p->fsm) {
    case INFLATE : {
      if (p->pcnt < p->pw) {
        PamSetU(p,1);
      }
      else {
        PamSetU(p,0);
        p->fsm = REFRACT;
        p->pcnt = 0;
      }
      p->pcnt++;
      break;
    }
    case DEFLATE : {
      if (p->pcnt < p->pw) {
        PamSetU(p,-1);
      }
      else {
        PamSetU(p,0);
        p->fsm = REFRACT;
        p->pcnt = 0;
      }
      p->pcnt++;
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

void PamActionPulse(pam_t* p) {
  static const uint32_t pulse_width = 5;
  static const uint32_t pulse_period = 50;

  switch (p->fsm) {
    case INFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) < 0) {
        p->fsm = REFRACT;
        p->pcnt = 0;
        PamSetU(p,0);
      } else {
        if ((p->pcnt%pulse_period) < pulse_width) {
          PamSetU(p,1);
        }
        else {
          PamSetU(p,0);
        }
      }
      p->pcnt++;
      break;
    }
    case DEFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) > 0) {
        p->fsm = REFRACT;
        p->pcnt = 0;
        PamSetU(p,0);
      } else {
        if ((p->pcnt%pulse_period) < pulse_width) {
          PamSetU(p,-1);
        }
        else {
          PamSetU(p,0);
        }
      }
      p->pcnt++;
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
