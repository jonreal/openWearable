#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile fix16_t inter_force;
  volatile fix16_t flx_strtch;
  volatile fix16_t dflx_strtch;
  volatile fix16_t flx_p;
  volatile fix16_t ext_p;
  volatile fix16_t flx_pd;
  volatile fix16_t ext_pd;
  volatile int16_t flx_u;
  volatile int16_t ext_u;
} state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;

#endif
