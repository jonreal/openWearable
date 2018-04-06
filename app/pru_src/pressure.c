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
#include <stdint.h>
#include "i2cdriver.h"

void PressureSensorInit(void) {
  uint8_t buffer[2] = {0};

  // Check Status Bits
  i2cRxBurstNoReg(SENSOR_1, 2, buffer);
}

fix16_t PressureSensorSample(uint8_t addr) {
  uint8_t buffer[4] = {0};
  i2cRxBurstNoReg(SENSOR_1, 4, buffer);

  uint32_t bits = (((uint32_t) (buffer[0] & ~0xC0)) << 8)
                  | ((uint32_t) buffer[1]);

  return fix16_smul(fix16_from_int(bits),BITS2PSI);

}