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

#include "encoder.h"
#include <stdint.h>
#include <stdlib.h>
#include "spidriver.h"

static fix16_t EncoderCnts2Degs(uint16_t cnts)
{
  fix16_t angleDeg = fix16_smul(
      fix16_smul(fix16_from_int(cnts), FIX16_360_DIV_4096),0xFFFF0000);

//    angleDeg = fix16_ssub(angleDeg, FIX16_360);
  if (angleDeg < fix16_smul(FIX16_180,0xFFFF0000))
    angleDeg = fix16_sadd(angleDeg, FIX16_360);

  return angleDeg;
}

encoder_t* EncoderInit(uint8_t spi_ch) {
  encoder_t* enc = malloc(sizeof(encoder_t));
  enc->spi_ch = spi_ch;
  enc->angle = 0;
  if (spiXfer(spi_ch, 0x0) == 0xA5)
    enc->active = 1;
  else
    enc->active = 0;
  return enc;
}

void EncoderCleanUp(encoder_t* enc) {
  free(enc);
}

void EncoderUpdate(encoder_t* enc) {
  uint8_t rx = 0;
  uint8_t MSB = 0;
  uint8_t LSB = 0;

  // Issue read command (0x10)
  rx = spiXfer(enc->spi_ch, 0x10);

  // Wait till recieve ack. (0x10)
  while(rx != 0x10){
    rx = spiXfer(enc->spi_ch, 0x00);
  }

  // RX data
  MSB = spiXfer(enc->spi_ch, 0x00);
  LSB = spiXfer(enc->spi_ch, 0x00);

  enc->angle =  EncoderCnts2Degs((uint16_t) ((MSB << 8) | (LSB & 0xFF)));
}

void EncoderTare(encoder_t* enc) {
  uint8_t rx = 0;

  // Issue read command (0x70)
  rx = spiXfer(enc->spi_ch, 0x70);

  // Wait till recieve ack. (0x80)
  while(rx != 0x80)
    rx = spiXfer(enc->spi_ch, 0x00);
}

fix16_t EncoderGetAngle(encoder_t* enc) {
  return enc->angle;
}
