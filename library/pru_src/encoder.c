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

#include "spidriver.h"
#include "encoder.h"


int8_t encoderInit(void)
{
  if (spiXfer(SPI_ENCODER_CHAN, 0x0) == 0xA5)
    return 0;

  return -1;
}

void encoderCleanUp(void)
{

}

void encoderSample(volatile fix16_t *pos)
{
  uint8_t rx = 0;
  uint8_t MSB = 0;
  uint8_t LSB = 0;

  // Issue read command (0x10)
  rx = spiXfer(SPI_ENCODER_CHAN, 0x10);

  // Wait till recieve ack. (0x10)
  while(rx != 0x10){
    rx = spiXfer(SPI_ENCODER_CHAN, 0x00);
  }

  // RX data
  MSB = spiXfer(SPI_ENCODER_CHAN, 0x00);
  LSB = spiXfer(SPI_ENCODER_CHAN, 0x00);

  *pos =  encoderCnts2Degs((uint16_t) ((MSB << 8) | (LSB & 0xFF)));
}

fix16_t encoderCnts2Degs(uint16_t cnts)
{
  fix16_t angleDeg = fix16_smul(fix16_from_int(cnts), FIX16_360_DIV_4096);

  if (angleDeg > FIX16_180)
    angleDeg = fix16_ssub(angleDeg, FIX16_360);

  return angleDeg;
}

void encoderSetZeroAngle(void)
{
  uint8_t rx = 0;

  // Issue read command (0x70)
  rx = spiXfer(SPI_ENCODER_CHAN, 0x70);

  // Wait till recieve ack. (0x80)
  while(rx != 0x80)
    rx = spiXfer(SPI_ENCODER_CHAN, 0x00);
}
