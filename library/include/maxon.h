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

#ifndef _MAXON_H_
#define _MAXON_H_

#include <stdint.h>
#include "fix16.h"

typedef struct{
  fix16_t velocity;
  fix16_t current;
  fix16_t u;
  fix16_t duty;
} motor_state_t;

typedef struct {
  uint8_t enable_pin;
  uint8_t adc_vel_ch;
  uint8_t adc_cur_ch;
  fix16_t G;
  fix16_t Kt;
  fix16_t Kv;
  fix16_t max_current;
  fix16_t max_velocity;
  volatile motor_state_t state;
} motor_t;

motor_t* MaxonMotorInit(
                  uint8_t enable_pin, uint8_t adc_cur_ch, uint8_t adc_vel_ch,
                  fix16_t G, fix16_t Kt, fix16_t Kv,
                  fix16_t max_current, fix16_t max_velocity);
void MaxonUpdate(motor_t* m);
void MaxonSetCurrent(motor_t* m, fix16_t u);
void MaxonAction(motor_t* m);
motor_state_t MaxonGetState(motor_t* m);
void MaxonMotorFree(motor_t* m);

//void motorCleanUp(void);
//uint16_t motorCurrent2CmpValue(fix16_t u);
//void motorSetCurrent(fix16_t u, volatile uint32_t *motorPwmCmpValue);
//void motorEnable(void);
//void motorDisable(void);

#endif
