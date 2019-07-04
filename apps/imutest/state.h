#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "bno055.h"

// State
typedef struct {
  volatile uint32_t time;
  volatile euler_t euler1;
  volatile euler_t euler2;
 } state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
