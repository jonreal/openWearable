#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile fix16_t p_m;
  volatile fix16_t p_d;
  volatile int16_t u;
  volatile uint16_t sync_val;
} state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;

#endif