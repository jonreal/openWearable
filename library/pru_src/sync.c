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

sync_t* SyncInitChan(uint8_t pin_) {
  sync_t* sync = malloc(sizeof(sync_t));
  sync->pin = pin_;
  sync->state = 0;
  return sync;
}

void SyncFreeChan(sync_t* sync){
  free(sync);
}

void SyncOutHigh(sync_t* sync) {
  sync->state = 1;
  __R30 |= (1 << sync->pin);
}

void SyncOutLow(sync_t* sync) {
  sync->state = 0;
  __R30 &= ~(1 << sync->pin);
}

void SyncInUpdate(sync_t* sync) {
  if( (__R31 & (1 << sync->pin)) == (1 << sync->pin)){
    sync->state = 1;
  } else {
    sync->state = 0;
  }
}


uint32_t SyncGetState(const sync_t* sync) {
  return sync->state;
}



