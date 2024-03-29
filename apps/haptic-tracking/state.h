#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "maxon.h"

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
  volatile fix16_t tau_active;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t Jvirtual;
  fix16_t bvirtual;
  fix16_t kvirtual;
  fix16_t targets[20];
  fix16_t fd;
  fix16_t Td;
  fix16_t k2PiFd;
  fix16_t Np;
} param_mem_t;


#endif
