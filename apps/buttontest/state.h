#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile int32_t fsm_state;
  volatile fix16_t pm_pro;
  volatile fix16_t pm_sup;
  volatile fix16_t pd_pro;
  volatile fix16_t pd_sup;
  volatile uint32_t sync;
 } state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t pd_sup;
  fix16_t pd_pro;
  uint32_t hold_cnt;
} param_mem_t;


#endif
