#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile fix16_t emg1_raw;
  volatile fix16_t emg1_rect;
  volatile fix16_t emg1_nlb;
  volatile fix16_t emg2_raw;
  volatile fix16_t emg2_rect;
  volatile fix16_t emg2_nlb;
  volatile fix16_t p1_m;
  volatile fix16_t dp1_m;
  volatile fix16_t p1_d;
  volatile fix16_t p2_m;
  volatile fix16_t dp2_m;
  volatile fix16_t p2_d;
  volatile int32_t u1;
  volatile int32_t u2;
  volatile fix16_t angle;
  volatile fix16_t angle_d;
  volatile uint32_t sync_val;
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
