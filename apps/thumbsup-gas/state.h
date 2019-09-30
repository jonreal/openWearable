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
  volatile uint32_t status;
} cpudata_t;

// --- state
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile uint32_t sync;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
