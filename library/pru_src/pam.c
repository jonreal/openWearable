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


pam_t* PamInitMuscle(uint8_t bus_address,
                     uint8_t bus_channel,
                     uint8_t sensor_address,
                     uint32_t in_pin,
                     uint32_t out_pin,
                     uint32_t divisor,
                     fix16_t threshold) {

  pam_t* pam = malloc(sizeof(pam_t));
  pam->mux = MuxI2cInit(bus_address);
  pam->sensor = PressureSensorInit(sensor_address);
  pam->channel = bus_channel;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->fs_div = divisor;
  pam->thr = threshold;
  pam->cnt = 0;
  pam->prev_cmd = 0;

  PamSamplePressure(pam);
  return pam;
}

void PamFreeMuscle(pam_t* pam) {
   __R30 |= (1 << pam->lp_pin);
   __R30 &= ~(1 << pam->hp_pin);
  __delay_cycles(1000000000);
   __R30 &= ~(1 << pam->lp_pin);
  PressureSensorFree(pam->sensor);
  MuxI2cFree(pam->mux);
  free(pam);
}

fix16_t PamSamplePressure(const pam_t* pam){
  fix16_t rtn;
  MuxI2cSetChannel(pam->mux, pam->channel);
  __delay_cycles(1100);
  rtn = PressureSensorSample(pam->sensor);
  __delay_cycles(500);
  return rtn;
}

void PamUpdateControl(pam_t* pam,
                      const volatile fix16_t* p_m,
                      const volatile fix16_t* p_d,
                      volatile int16_t* valve_cmd) {

  fix16_t error = (*p_d + pam->thr) - *p_m;
  //fix16_t error = *p_d - *p_m;

  if (pam->cnt == (pam->fs_div - 1)){
    pam->cnt = 0;
    if ((fix16_sadd(error, pam->thr) >= 0)
        && (fix16_ssub(error, pam->thr) <= 0)) {
      *valve_cmd = 0;
    } else if (fix16_ssub(error, pam->thr) > 0) {
      *valve_cmd = 1;
    } else if (fix16_sadd(error, pam->thr) < 0) {
      *valve_cmd = -1;
    }
  } else {
    // Enforce pusle width min/max
    if (pam->cnt <= 1) {
      *valve_cmd = pam->prev_cmd;
    } else {
      *valve_cmd = 0;
    }
    (pam->cnt)++;
  }

  // ignore threshold if p_d = 0
  if (*p_d == 0)
    *valve_cmd = -1;

  PamSetValveCommand(pam, valve_cmd);
  pam->prev_cmd = *valve_cmd;
}

static void PamSetValveCommand(const pam_t* pam,
                               const volatile int16_t* valve_cmd) {
  switch (*valve_cmd) {
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
