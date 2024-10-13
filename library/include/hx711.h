/* Copyright 2024 Jonathan Realmuto

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

#ifndef _HX711_H_
#define _HX711_H_

#include <stdint.h>
#include "fix16.h"

#define HX711_DELAY_CYCLES (10)

typedef enum {
  HX711_GAIN_128 = 1,  // Channel A, Gain 128
  HX711_GAIN_64 = 3,   // Channel A, Gain 64
  HX711_GAIN_32 = 2    // Channel B, Gain 32
} hx711_gain_t;

typedef struct {
  uint8_t clk_pin;
  uint8_t dout_pin;
  uint8_t gain;
  int32_t value;
} hx711_t;

// Digital input reg. ---------------------------------------------------------
volatile register uint32_t __R31;
volatile register uint32_t __R30;

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debug_buff;

hx711_t* Hx711InitLoadCell(
  uint8_t clk_pin,
  uint8_t dout_pin,
  hx711_gain_t gain
);
void Hx711Free(hx711_t* hx711);
int32_t Hx711Sample(hx711_t* hx711);

#endif
