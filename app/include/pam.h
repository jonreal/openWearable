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
#include "hscsann150pg2a3.h"
#include "pca9548a.h"

// Digital Ouput Register
volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

typedef struct {
  i2cmux_t* mux;
  pressure_sensor_t* sensor;
  uint8_t channel;
  uint8_t hp_pin;                 // high pressure pin
  uint8_t lp_pin;                 // low pressure pin
  uint8_t fs_div;                 // sample freq divider
  uint8_t cnt;                    // loop cnt (for downsampling)
  int8_t prev_cmd;                // prev command
  fix16_t thr;                    // threshold for valve control
} pam_t;


// Public
pam_t* PamInitMuscle(uint8_t bus_address,
                     uint8_t bus_channel,
                     uint8_t sensor_address,
                     uint32_t in_pin,
                     uint32_t out_pin,
                     uint32_t divisor,
                     fix16_t threshold);
void PamFreeMuscle(pam_t* pam);
fix16_t PamSamplePressure(const pam_t* pam);
void PamUpdateControl(pam_t* pam,
                      const volatile fix16_t* p_m,
                      const volatile fix16_t* p_d,
                      volatile int16_t* valve_cmd);

// Private
static void PamSetValveCommand(const pam_t* pam,
                               const volatile int16_t* valve_cmd);

#endif /* _PAM_H_ */
