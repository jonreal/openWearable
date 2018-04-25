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

#ifndef _PAM_H_
#define _PAM_H_

#include <stdint.h>
#include "fix16.h"
#include "pressure.h"

// Pin config
#define FLEX_V_HP 9
#define FLEX_V_LP 11
#define EXT_V_HP 10
#define EXT_V_LP 8

// Sensor config
#define SENSOR_ADD 0x28
#define MUX_SEL 6
#define FIX16_1 0x10000

typedef struct {
  sensor_i2c_t* sensor;     // sensor struct (see pressure.h)
  uint8_t hp_pin;           // high pressure pin
  uint8_t lp_pin;           // low pressure pin
  uint8_t fs_div;           // sample freq divider
  uint8_t res;              // pad
  fix16_t thr;              // threshold for valve control
} pam_t;


pam_t* PamInitMuscle(uint8_t address, uint8_t mux_pin, uint8_t mux_ch,
                      uint32_t in_pin, uint32_t out_pin,
                      uint32_t div, fix16_t threshold);
void PamFreeMuscle(pam_t* pam);
void PamSamplePressure(const pam_t* pam, volatile fix16_t* p_m);
void PamUpdateControl(const pam_t* pam, const volatile fix16_t* p_m,
                        const volatile fix16_t* p_d,
                        volatile int32_t* valve_cmd);
static void PamSetValveCommand(const pam_t* pam,
                                const volatile int32_t* valve_cmd);



#endif /* _PAM_H_ */
