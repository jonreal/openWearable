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

#ifndef _SYNC_H_
#define _SYNC_H_

#include <stdint.h>

typedef struct {
  uint8_t pin;
  uint8_t state;
} sync_t;

// Digital input reg. ---------------------------------------------------------
volatile register uint32_t __R31;
volatile register uint32_t __R30;

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debug_buff;

// Prototypes -----------------------------------------------------------------
//uint16_t viconSync(void);
sync_t* SyncInitChan(uint8_t pin_);
void SyncFreeChan(sync_t* sync);
void SyncOutHigh(sync_t* sync);
void SyncOutLow(sync_t* sync);
void SyncInUpdate(sync_t* sync);
uint32_t SyncGetState(const sync_t* sync);


#endif


