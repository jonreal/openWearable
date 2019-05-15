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

#include "pressure.h"
#include <stdlib.h>
#include "i2cdriver.h"

sensor_i2c_t* PressureSensorInit(uint8_t address, uint8_t mux_pin,
                                    uint8_t mux_ch) {
  sensor_i2c_t* sens = malloc(sizeof(sensor_i2c_t));
  sens->addr = address;
  sens->mux = MuxInit(mux_pin, mux_ch);

  uint8_t buffer[2] = {0};
  MuxSetCh(sens->mux);
  i2cRxBurstNoReg(sens->addr, 2, buffer);
  return sens;
}

fix16_t PressureSensorSample(const sensor_i2c_t* sens) {
  uint8_t buffer[4] = {0};

  MuxSetCh(sens->mux);
  i2cRxBurstNoReg(sens->addr, 4, buffer);

  uint32_t bits = (((uint32_t) (buffer[0] & ~0xC0)) << 8)
                  | ((uint32_t) buffer[1]);

  return fix16_smul(fix16_ssub(fix16_from_int(bits),FIX16_Kp),FIX16_Mp);
}


void PressureSensorFree(sensor_i2c_t* sens){
  MuxFree(sens->mux);
  free(sens);
}

static mux_t* MuxInit(uint8_t mux_pin, uint8_t mux_ch){
  mux_t* m = malloc(sizeof(mux_t));
  m->pin = mux_pin;
  m->ch = mux_ch;
  debug_buff[0] = m->pin;
  return m;
}

static void MuxFree(mux_t* m) {
  free(m);
}

static void MuxSetCh(const mux_t* m) {
  __delay_cycles(1000);
  switch (m->ch) {
    case 0:
      __R30 &= ~(1 << m->pin);
      __delay_cycles(1000);
      break;
    case 1:
      __R30 |= (1 << m->pin);
      __delay_cycles(1000);
      break;
  }
}
