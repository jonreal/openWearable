#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "maxon.h"
#include "pam.h"
#include "emg.h"

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
  volatile motor_state_t motor;
  volatile fix16_t dx;
  volatile fix16_t ddx;
  volatile fix16_t tau_active;
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
  volatile fix16_t triggersignal;
  volatile fix16_t triggersignal_myo;
  volatile emg_state_t emg1_state;
  volatile emg_state_t emg2_state;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  uint32_t Td;  // cycle period (in clock cnts)
  uint32_t Np; // number of cycles (or ballistic targets)
  fix16_t targets[40];
  fix16_t Jvirtual;
  fix16_t bvirtual;
  fix16_t kvirtual;
  fix16_t P0;
  fix16_t threshold;
  fix16_t dP;
} param_mem_t;


#endif
