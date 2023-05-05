#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "maxon.h"
#include "pam.h"

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
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
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
  fix16_t Pd1;
  fix16_t Pd2;
  fix16_t step;
  fix16_t repeat;
} param_mem_t;


#endif
