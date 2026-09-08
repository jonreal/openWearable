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

#ifndef _MEM_TYPES_
#define _MEM_TYPES_

#include <stddef.h>
#include "state.h"
#include "error.h"

#define PRU_CTRL_BASE     0x00022000
//#define STATE_BUFF_LEN    149
#define STATE_BUFF_LEN    96

// Debug pins
#define PRU0_DEBUG_PIN  5
#define PRU1_DEBUG_PIN  0

// Structures ----------------------------------------------------------------

// Owned-words signaling labels (enum, not #define). Explicit masks: this struct
// is a clpru<->gcc shared-memory ABI, so no bitfields. App command/signal bits
// use 8..30 (named per app in its state.h); bit 31 avoided (int enum, --c99).
enum arm_bit {            // written only by the A8
  ARM_RUN   = 1u << 0,    // enable the control loop
  ARM_ARMED = 1u << 1,    // command channel: changes accepted only while set
};
enum pru_bit {            // written only by the owning PRU (pru0 / pru1)
  PRU_READY = 1u << 0,    // driver + app init complete
  PRU_FAULT = 1u << 1,    // reserved; fault today via shared_mem_t.fault
};

// Shared Memory -> mapped to SRAM
typedef struct {
  volatile uint32_t cbuff_index;
  state_t state[STATE_BUFF_LEN];
  cpudata_t cpudata;
  volatile error_t fault;      // latched fault (fail-stop); ARM reads .raised
  // Owned-words signaling: ONE writer per word; every core reads all of it.
  volatile uint32_t arm;       // A8-owned  : ARM_RUN + A8->PRU commands
  volatile uint32_t pru0;      // PRU0-owned: PRU_READY/FAULT + signals
  volatile uint32_t pru1;      // PRU1-owned: PRU_READY/FAULT + signals
  volatile uint32_t pru0_seq;  // PRU0-owned: monotonic tick barrier
  volatile uint32_t pru1_seq;  // PRU1-owned: monotonic tick barrier
} shared_mem_t;

// Lookuptable
typedef struct {
  int16_t lut[1000];
} lut_mem_t;

// Pointers to memory
typedef struct {
  shared_mem_t* s;
  param_mem_t* p;
  lut_mem_t* l;
} pru_mem_t;

// Counters
typedef struct {
  uint32_t frame;
  uint32_t index;
} pru_count_t;

#endif

