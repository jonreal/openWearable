#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "maxon.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t varcpu;
} cpudata_t;

// --- State
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t x;
  volatile motor_state_t motor;
  volatile uint32_t sync;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  uint32_t Tf;   // total time (ms)
  fix16_t f0;   // start freq.
  fix16_t f1;   // end freq;
  fix16_t A;    // current amplitude
} param_mem_t;


#endif
