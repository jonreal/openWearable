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
#include "hscsann150pg2a3.h"    // pressure sensor
#include "pca9548a.h"           // i2c multiplexer

typedef struct {
  fix16_t p_d;             // desired pressure
  fix16_t p_m;             // muscle pressure
  int32_t u;                // current command
} pam_state_t;


typedef struct {
  pressure_sensor_t* sensor;
  uint8_t hp_pin;                   // high pressure pin
  uint8_t lp_pin;                   // low pressure pin
  volatile uint8_t flag;
  volatile pam_state_t s;
} pam_t;

typedef struct {
  pressure_sensor_t* sensor;
  volatile fix16_t pressure;
} reservoir_t;


reservoir_t* PamReservoirInit(pressure_sensor_t* sens);
void PamReservoirFree(reservoir_t* reservoir);
void PamReservoirUpdate(reservoir_t* reservoir);
fix16_t PamReservoirGetPressure(const reservoir_t* reservoir);

pam_t* PamInitMuscle(pressure_sensor_t* sens,
                     uint32_t in_pin,
                     uint32_t out_pin);
void PamMuscleFree(pam_t* pam);
void PamUpdate(pam_t* pam);
void PamActionSimple(pam_t* p);
pam_state_t PamGetState(const pam_t* p);
void PamSetPd(pam_t* pam, fix16_t Pd);
void PamSetU(pam_t* pam, int8_t u);

#endif /* _PAM_H_ */
