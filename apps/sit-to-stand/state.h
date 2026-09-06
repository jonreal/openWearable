#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "../../library/pru/include/pam.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t cpuvar;
} cpudata_t;

// --- State
typedef struct {
  volatile uint32_t frame;        // monotonic loop counter (PRU0 writes pru_count_t.frame)
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

// Owned-words command/signal bits (app-specific; bits 8+).
enum { CMD_TRIAL      = 1u << 8 };   // A8->PRU (arm):  start a trial
enum { SIG_TRIAL_DONE = 1u << 8 };   // PRU->A8 (pru1): trial complete


#endif
