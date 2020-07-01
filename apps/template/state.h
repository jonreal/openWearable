#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t cpuVar;
} cpudata_t;

// --- State
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  fix16_t pruVar1;
  uint32_t pruVar2;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
