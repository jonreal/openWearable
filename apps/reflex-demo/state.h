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
  volatile uint32_t frame;
  cpudata_t cpudata;
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
  volatile fix16_t triggersignal;
  volatile uint32_t pot1;
  volatile uint32_t pot2;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t P0;
  volatile fix16_t threshold;
  volatile fix16_t dP;
  volatile fix16_t Pmax;        // EP controller pressure ceiling (UI-mutable)
} param_mem_t;

// A8 -> PRU command bits (owned-words signaling, app bits start at 8).
enum { CMD_REFLEX = 1u << 8 };  // [r] toggles the reflex on/off


#endif
