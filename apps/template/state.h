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
  volatile uint32_t pru0var;
  volatile uint32_t pru1var;
  cpudata_t cpudata;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


// -- Shared Memory layout -- must match .cmd files
static const uintptr_t global_sharedram_base = 0x0b010000;
static const uintptr_t sharedram_base = 0x00010000;

static const uintptr_t state_ram_offset = 0x0;
static const uintptr_t param_ram_offset = 0xE000;
static const uintptr_t lut_ram_offset = 0xF000;

#endif
