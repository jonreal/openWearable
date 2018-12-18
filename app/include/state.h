#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile fix16_t emg;
  volatile fix16_t emg_nlb;
  volatile fix16_t p_m;
  volatile fix16_t dp_m;
  volatile fix16_t p_d;
  volatile fix16_t angle;
  volatile fix16_t angle_d;
  volatile int16_t u;
  volatile uint16_t sync_val;
} state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t p_d_static;
  fix16_t theta_d;
} param_mem_t;

#endif
