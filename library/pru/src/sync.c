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

#include "sync.h"
#include <stdlib.h>

//// Sync pin goes high
//uint16_t viconSync(void)
//{
//  if( (__R31 & (1 << SYNC_PIN)) == (1 << SYNC_PIN)){
//    return 1;
//  }
//  return 0;
//}

sync_t* SyncInitChan(uint8_t pin_) {
  sync_t* sync_ch = malloc(sizeof(sync_t));
  sync_ch->pin = pin_;
  sync_ch->state = 0;
  return sync_ch;
}

void SyncFreeChan(sync_t* sync_ch){
  free(sync_ch);
}

void SyncOutHigh(sync_t* sync_ch) {
  sync_ch->state = 1;
  __R30 |= (1 << sync_ch->pin);
}

void SyncOutLow(sync_t* sync_ch) {
  sync_ch->state = 0;
  __R30 &= ~(1 << sync_ch->pin);
}

uint8_t SyncOutState(const sync_t* sync_ch) {
  return sync_ch->state;
}



