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

#ifndef _ADCDRIVER_H_
#define _ADCDRIVER_H_

#include <stdint.h>

// ---- Global ----
extern volatile uint32_t *debugBuffer;


void adcInit(void);
//void adcSampleAll(void);
uint32_t adcSample(char ch);
void adcCleanup(void);

//#define MUX_SEL_PIN     7
//
//
///* --- Digital output reg. */
//volatile register uint32_t __R30;
//
///* ---- Global ---- */
//extern volatile uint32_t *debugBuffer;
//
///* ---- Prototypes ---- */
//void adcInit(void);
//void adcSample_1(int16_t adc[8]);
//void adcSample_2(int16_t adc[8]);
//void adcSample_3(int16_t adc[8]);
//void adcCleanUp(void);

#endif
