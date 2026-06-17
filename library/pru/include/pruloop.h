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

#ifndef _PRULOOP_H_
#define _PRULOOP_H_

#include "mem_types.h"

// Global ---------------------------------------------------------------------
extern volatile uint32_t* debug_buff;

// Hook context (MuJoCo-style split: a const `view` of everything the control
// loop may read, and a mutable `io` surface it writes this tick). The PRU main
// loop assembles both and passes them to every Update hook, so a hook's
// signature states exactly what it may read vs. write -- a write through
// `view` fails to compile.
typedef struct {
  const pru_count_t* c;   // loop counters: frame (monotonic) / index (ring slot)
  const param_mem_t* p;   // parameters
  const lut_mem_t*   l;   // feed-forward lookup table
} pru_view_t;

typedef struct {
  state_t*   s;           // this iteration's state slot (read/write)
  pru_ctl_t* ctl;         // flow-control register (utility bits)
} pru_io_t;

// Prototypes ---------------------------------------------------------------
void Pru0Init(pru_mem_t* mem);
void Pru0UpdateState(const pru_view_t* view, pru_io_t* io);
void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io);
void Pru0Cleanup(void);

void Pru1Init(pru_mem_t* mem);
void Pru1UpdateState(const pru_view_t* view, pru_io_t* io);
void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io);
void Pru1Cleanup(void);

uint8_t PruGetCtlBit(const pru_ctl_t* ctl_, uint8_t nbit);
void PruSetCtlBit(pru_ctl_t* ctl_, uint8_t nbit);
void PruClearCtlBit(pru_ctl_t* ctl_, uint8_t nbit);

// inline helpers here
inline uint8_t PruGetCtlBit(const pru_ctl_t* ctl_, uint8_t nbit) {
  return ((ctl_->bit.utility & (1 << nbit)) == (1 << nbit));
}

inline void PruSetCtlBit(pru_ctl_t* ctl_, uint8_t nbit) {
  ctl_->bit.utility |= (1 << nbit);
}

inline void PruClearCtlBit(pru_ctl_t* ctl_, uint8_t nbit) {
  ctl_->bit.utility &= ~(1 << nbit);
}
#endif
