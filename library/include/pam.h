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
#include "filter.h"
#include "hscsann150pg2a3.h"    // pressure sensor
#include "pca9548a.h"           // i2c multiplexer

// Digital Ouput Register
volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

typedef struct {
  pressure_sensor_t* sensor;
  uint8_t hp_pin;                   // high pressure pin
  uint8_t lp_pin;                   // low pressure pin
  volatile int8_t u;                         // current command
  volatile fix16_t p_m;             // muscle pressure
  volatile fix16_t p_d;             // desired pressure
  volatile fix16_t p_max;
  volatile fix16_t reflex;
} pam_t;

typedef struct {
  uint8_t pulsewidth;
  uint8_t period;
  uint8_t pulseonset;
  int8_t sign;
  fix16_t threshold;
  volatile uint8_t cnt;
  volatile uint8_t on;
} reflex_t;


typedef struct {
  pressure_sensor_t* sensor;
  volatile fix16_t p_res;
} reservoir_t;

// Public
pam_t* PamInitMuscle(pressure_sensor_t* sens,
                     uint32_t in_pin,
                     uint32_t out_pin);
reservoir_t* PamInitReservoir(pressure_sensor_t* sens);
void PamMuscleFree(pam_t* pam);
void PamReservoirFree(reservoir_t* reservoir);


void PamUpdateSensors(pam_t* pam, iir_filt_t* f);
int8_t PamReflex(pam_t* pam, reflex_t* reflex);

reflex_t* PamInitReflex(uint8_t pw, uint8_t T, uint8_t po, int8_t sign,
                        fix16_t thresh);
void PamMyReflex(pam_t* pam, uint8_t period,  uint8_t pulsewidth,
                  uint8_t pulseonset);
void PamAntagonistReflex(pam_t* p1, pam_t* p2,
                        iir_filt_t* f1, iir_filt_t* f2,
                        uint8_t period,  uint8_t pulsewidth,
                        uint8_t pulseonset);

fix16_t PamGetPm(pam_t* pam);
fix16_t PamGetPd(pam_t* pam);
void PamSetPd(pam_t* pam, fix16_t Pd);

void PamSetU(pam_t* pam, int8_t u);
void PamUpdateControl(pam_t* pam);

int16_t PamGetU(pam_t* pam);
fix16_t PamGetPmax(pam_t* pam);


fix16_t PamSampleReservoirPressure(const reservoir_t* reservoir);


//void PamThresholdControl(pam_t* pam,
//                          const volatile fix16_t* p_m,
//                          const volatile fix16_t* p_d,
//                          const fix16_t thr,
//                          const uint8_t fs_div,
//                          volatile int16_t* valve_cmd);

//void PamReflexControl(pam_t* pam,
//                      fix16_t* pm,
//                      fix16_t* p0, fix16_t dp);
#endif /* _PAM_H_ */
