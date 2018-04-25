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

#ifndef _PRESSURESENSOR_H_
#define _PRESSURESENSOR_H_

#include "fix16.h"

// P = (bits - res * 0.1) * Pmax/ (res*0.8)
//   = (bits - 2^14 * 0.1) * Pmax/(2^14*0.8)
//   = (bits - Kp) * Mp; K = 2^14*0.1, M = 1.05420 kPa / (0.8 * 2^14)
#define FIX16_Kp 0x6666668
// kPa
//#define FIX16_Mp 0xD7E6
// psi
#define FIX16_Mp 0x2EE

typedef struct {
  uint8_t pin;          // mux select pin
  uint8_t ch;           // mux channel (1 | 0)
} mux_t;

typedef struct {
  uint8_t addr;         // i2c address
  uint8_t res;          // pad
  mux_t* mux;           // mux struct
} sensor_i2c_t;

// Global ---------------------------------------------------------------------
volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

// Prototypes -----------------------------------------------------------------
sensor_i2c_t* PressureSensorInit(uint8_t address, uint8_t mux_pin,
                                    uint8_t mux_ch);
void PressureSensorFree(sensor_i2c_t* sens);
fix16_t PressureSensorSample(const sensor_i2c_t* sens);
static mux_t* MuxInit(uint8_t mux_pin, uint8_t mux_ch);
static void MuxFree(mux_t* m);
static void MuxSetCh(const mux_t* m);

#endif
