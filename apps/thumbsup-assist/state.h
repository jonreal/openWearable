#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "pam.h"
#include "bno055_arm.h"


//
//  packing can be issue -- use 32bit types
//

// --- cpudata struct
typedef struct {
  volatile euler_t imu1;
  volatile euler_t imu2;
} cpudata_t;

// --- state
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
  volatile fix16_t triggersignal;
  volatile uint32_t buttons;
  volatile uint32_t sync;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  volatile fix16_t p_0;
  volatile fix16_t dp;
  volatile fix16_t thr;
} param_mem_t;


#endif
