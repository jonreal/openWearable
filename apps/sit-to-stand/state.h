#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "pam.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t cpuvar;
} cpudata_t;

// --- State
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t p_res;
  volatile pam_state_t pam_state;
  volatile int32_t load_r;
  volatile int32_t load_l;
  volatile uint32_t sync_state;
  volatile uint32_t status;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t Ptarget;
  uint32_t Tdelay;
  uint32_t Ttrial;
} param_mem_t;


#endif
