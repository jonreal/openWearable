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

#include "hx711.h"
#include <stdlib.h>


#define HX711_DELAY __delay_cycles(200)


hx711_t* Hx711InitLoadCell(
  uint8_t clk_pin, uint8_t dout_pin, hx711_gain_t gain
) {
  hx711_t* hx711 = (hx711_t*)malloc(sizeof(hx711_t));
  hx711->clk_pin = clk_pin;
  hx711->dout_pin = dout_pin;
  hx711->gain = gain;
  hx711->value = 0;

  // Set the clock pin low initially
  __R30 &= ~(1 << clk_pin);

  for (int i = 0; i < 24 + hx711->gain; i++) {
    __R30 |= (1 << hx711->clk_pin);
    HX711_DELAY;
    __R30 &= ~(1 << hx711->clk_pin);
    HX711_DELAY;
  }

  return hx711;
}

// Free the HX711 object
void Hx711Free(hx711_t* hx711) {
  if (hx711) {
    free(hx711);
  }
}


// Read a 24-bit value from the HX711 using __R30 and __R31 directly
int32_t Hx711Sample(hx711_t* hx711) {
  uint32_t data = 0;

  // If data is not ready return previous value
  if (__R31 >> hx711->dout_pin & 1) {
    return hx711->value;
  }

  // Read 24 bits of data (3 bytes)
  for (int i = 0; i < 24; i++) {
      // Pulse the clock pin high
      __R30 |= (1 << hx711->clk_pin);
      HX711_DELAY;

      data = (data << 1) | (__R31 >> hx711->dout_pin & 1);

      // Pulse the clock pin low
      __R30 &= ~(1 << hx711->clk_pin);
      HX711_DELAY;
  }

  // Set the gain by pulsing the clock pin additional times gain
  for (int i = 0; i < hx711->gain; i++) {
    __R30 |= (1 << hx711->clk_pin);
    HX711_DELAY;
    __R30 &= ~(1 << hx711->clk_pin);
    HX711_DELAY;
  }

  // Convert to signed 32-bit value by checking the MSB
  if (data & 0x800000) {
    data |= 0xFF000000;  // If the 24th bit is set, sign extend the data
  }

  // Convert the raw data to fixed-point format and store in hx711->value
  hx711->value = (int32_t) data;
  return hx711->value;
}
