#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// packing can be issue -- use 32bit types

// --- State
typedef struct {
  volatile uint32_t time;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
