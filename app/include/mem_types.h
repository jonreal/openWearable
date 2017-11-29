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

#include "fix16.h"
#include "filter.h"
#include "nonlinBayes.h"

#define PRU_CTRL_BASE   0x00022000
#define PRU_CLK         (200000000)
#define SIZE_STATE_BUFF   149
#define NUM_FF_LT         1000
#define NUM_ADC           8
#define NUM_IMU           6

// Debug pins
#define PRU0_DEBUG_PIN  5
#define PRU1_DEBUG_PIN  8

// Structures ----------------------------------------------------------------
typedef struct{
  volatile uint32_t timeStamp;
  volatile fix16_t emg_raw;
} state_t;

// Shared Memory -> mapped to SRAM
typedef struct{
  state_t state[SIZE_STATE_BUFF];
  volatile uint32_t stateIndex;

  // Flow control
  union{
    volatile uint16_t cntrl;

    volatile struct{
      unsigned enable : 1;          // bit 0 (set by ARM and shadowed)
      unsigned pru0_done : 1;       // bit 1 (set by pru0, read/reset by pru1)
      unsigned pru1_done : 1;       // bit 2 (set by pru1, read/reset by pru0)
      unsigned shdw_enable : 1;     // bit 3 (shawdow reg. for enable)
      unsigned rsvd : 12;            // bits 11-15 reserved
   } cntrl_bit;
  };
} shared_mem_t;

// Parameter Struct -> mapped to pr0 DRAM
typedef struct{
  volatile uint32_t debugBuffer[10];
  volatile uint32_t frq_hz;
  volatile uint32_t frq_clock_ticks;
} param_mem_t;


// Feedforward lookup table -> mapped to pru1 DRAM
typedef struct{
  int16_t u_ff[NUM_FF_LT];
} lookUp_mem_t;

#endif

