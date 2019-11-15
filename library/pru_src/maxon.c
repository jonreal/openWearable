/* Copyright 2017 Jonathan Realmuto

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

#include "maxon.h"
#include <stdlib.h>
#include "pwmdriver.h"
#include "adcdriver.h"

volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

motor_t* MaxonMotorInit(
                  uint8_t enable_pin, uint8_t adc_cur_ch, uint8_t adc_vel_ch,
                  fix16_t G, fix16_t Kt, fix16_t Kv,
                  fix16_t max_current, fix16_t max_velocity) {
  motor_t* m = malloc(sizeof(motor_t));
  m->enable_pin = enable_pin;
  m->adc_vel_ch = adc_vel_ch;
  m->adc_cur_ch = adc_cur_ch;
  m->G = G;
  m->Kt = Kt;
  m->Kv = Kv;
  m->max_current = max_current;
  m->max_velocity = max_velocity;
  m->state.velocity = 0;
  m->state.current = 0;
  MaxonSetCurrent(m,0);
  pwmInit();
  __R30 |= (1 << m->enable_pin);
  MaxonAction(m);
  return m;
}

// In general, converting from bits to A (or rad/s):
//     y = mx + b
//     y = (max_value / 2^11) x - max_value
//     y = max_value [ (x/2^11) - 1 ]
//     y = max_value [ x*2^-11 - 1]
//     y = max_value [ (x * 0x20) - 1]   in fix16
// Note maxon settings should be:
// 0 V (0 Bits) -- -max_value
// Vadc V (2^12 bits) -- max_value
void MaxonUpdate(motor_t* m) {

//  m->state.velocity = AdcSampleChBits(m->adc_vel_ch);
//  m->state.current = AdcSampleChBits(m->adc_cur_ch);

  m->state.velocity = fix16_smul(m->max_velocity,
    fix16_ssub(fix16_smul(
              fix16_from_int(AdcSampleChBits(m->adc_vel_ch)),0x20),fix16_one));
  m->state.current = fix16_smul(m->max_current,
    fix16_ssub(fix16_smul(
              fix16_from_int(AdcSampleChBits(m->adc_vel_ch)),0x20),fix16_one));

}

void MaxonSetCurrent(motor_t* m, fix16_t u) {
  if (fix16_ssub(u,m->max_current) > 0)
    m->state.u = m->max_current;
  else if (fix16_sadd(u,m->max_current) < 0)
    m->state.u = -m->max_current;
  else
    m->state.u = u;
}

// 0%   - Imin
// 50%  - 0
// 100% - Imax
void MaxonAction(motor_t* m) {
  m->state.duty =  fix16_sadd(fix16_smul(fix16_sdiv(
            0x320000,m->max_current),m->state.u),0x320000);
  pwmSetDutyCycle(m->state.duty);
}

void MaxonMotorFree(motor_t* m) {
  pwmSetDutyCycle(0x320000);        // 50% duty
  __R30 &= ~(1 << m->enable_pin);
  free(m);
}

motor_state_t MaxonGetState(motor_t* m){
  return m->state;
}


