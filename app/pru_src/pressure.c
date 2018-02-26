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

#include <stdint.h>

#include "pressureSensor.h"
#include "i2cdriver.h"



void pressureSensorInit(void)
{
  uint8_t buffer[2] = {0};

  // Check Status Bits
  i2cRxBurstNoReg(SENSOR_1, 2, buffer);

  debug_buff[0] = buffer[0];
  debug_buff[1] = buffer[1];
}

fix16_t pressureSensorSample(uint8_t addr)
{
  uint8_t buffer[4] = {0};
  uint16_t pressureBytes = 0;
  uint16_t tempBytes = 0;

  // Check Status Bits
  i2cRxBurstNoReg(SENSOR_1, 4, buffer);

  pressureBytes =  (uint16_t) buffer[1] | (uint16_t)(buffer[0] << 2);

}
