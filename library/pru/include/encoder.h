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

#ifndef _ENCODER_H_
#define _ENCODER_H_

// Driver for CUI AMT20 series

#include "fix16.h"

#define FIX16_180           0xB40000
#define FIX16_360           0x1680000
#define FIX16_360_DIV_4096  0x1680

#define SPI_ENCODER_CHAN    0x1

typedef struct {
  volatile fix16_t angle;
  uint8_t active;
  uint8_t spi_ch;
} encoder_t;




// Global ---------------------------------------------------------------------
extern volatile uint32_t *debug_buff;

// Prototypes -----------------------------------------------------------------
encoder_t* EncoderInit(uint8_t spi_ch);
void EncoderCleanUp(encoder_t* enc);
void EncoderUpdate(encoder_t* enc);
void EncoderTare(encoder_t* enc);
fix16_t EncoderGetAngle(encoder_t* enc);

#endif
