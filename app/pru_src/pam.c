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


pam_t* PamInitMuscle(uint8_t address, uint8_t mux_pin, uint8_t mux_ch,
                     uint32_t in_pin, uint32_t out_pin, uint32_t freq,
                     fix16_t threshold) {
  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = PressureSensorInit(address, mux_pin, mux_ch);
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->fs_div = freq;
  pam->thr = threshold;
  return pam;
}

void PamFreeMuscle(pam_t* pam) {

   __R30 |= (1 << pam->lp_pin);
   __R30 &= ~(1 << pam->hp_pin);

  PressureSensorFree(pam->sensor);
  free(pam);
}

void PamSamplePressure(const pam_t* pam, volatile fix16_t* p_m){
  *p_m = PressureSensorSample(pam->sensor);
}

void PamUpdateControl(const pam_t* pam,
                      const volatile fix16_t* p_m,
                      const volatile fix16_t* p_d,
                      volatile int32_t* valve_cmd) {
  fix16_t error = *p_d - *p_m;

  if ((fix16_sadd(error, pam->thr) >= 0)
      && (fix16_ssub(error, pam->thr) <= 0)) {
    *valve_cmd = 0;
  } else if (fix16_ssub(error, pam->thr) > 0) {
    *valve_cmd = 1;
  } else if (fix16_sadd(error, pam->thr) < 0) {
    *valve_cmd = -1;
  }
  PamSetValveCommand(pam, valve_cmd);
}

static void PamSetValveCommand(const pam_t* pam,
                               const volatile int32_t* valve_cmd) {
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
