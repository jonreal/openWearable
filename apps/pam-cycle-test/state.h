#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "pam.h"

// --- cpudata struct
typedef struct {
  volatile uint32_t varcpu;
} cpudata_t;

// State
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
  volatile uint32_t sync;
  volatile uint32_t cyclecount;
} state_t;

// Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t Pmax;
  fix16_t Pmin;
  uint32_t Ton;
  uint32_t Toff;
  uint32_t Tcheck;
} param_mem_t;


#endif
