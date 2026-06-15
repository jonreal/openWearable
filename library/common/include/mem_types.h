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

#define PRU_CTRL_BASE     0x00022000
//#define STATE_BUFF_LEN    149
#define STATE_BUFF_LEN    96

// Debug pins
#define PRU0_DEBUG_PIN  5
#define PRU1_DEBUG_PIN  8

// Structures ----------------------------------------------------------------

// Flow control
typedef union {
  volatile uint16_t reg;
  volatile struct {
    unsigned enable : 1;            // bit 0 (set by ARM and shadowed)
    unsigned pru0_done : 1;         // bit 1 (set by pru0, read/reset by pru1)
    unsigned pru1_done : 1;         // bit 2 (set by pru1, read/reset by pru0)
    unsigned shdw_enable : 1;       // bit 3 (shawdow reg. for enable)
    unsigned utility : 12;          // 12 bits for user defined things
 } bit;
} pru_ctl_t;

// Shared Memory -> mapped to SRAM
typedef struct {
  volatile uint32_t cbuff_index;
  state_t state[STATE_BUFF_LEN];
  cpudata_t cpudata;
  pru_ctl_t pru_ctl;
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

