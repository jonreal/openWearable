#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// --- cpudata struct
typedef struct {
  volatile uint32_t varcpu;
} cpudata_t;

// --- state
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t x;
  volatile fix16_t xd;
  volatile uint32_t vsync;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  uint32_t Td;  // cycle period (in clock cnts)
  uint32_t Np; // number of cycles
} param_mem_t;


#endif
