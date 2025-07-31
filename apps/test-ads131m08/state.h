#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t cpuvar;
} cpudata_t;

// --- State
typedef struct {
  volatile uint32_t time;
  volatile uint32_t rx0;
  volatile uint32_t rx1;
  volatile uint32_t rx2;
  volatile uint32_t rx3;
  volatile uint32_t rx4;
  volatile uint32_t rx5;
  volatile uint32_t rx6;
  volatile uint32_t rx7;
  volatile uint32_t rx8;
  volatile uint32_t rx9;
  cpudata_t cpudata;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
