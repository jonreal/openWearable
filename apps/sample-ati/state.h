#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// packing can be issue -- use 32bit types


// --- cpudata struct
typedef struct {
  volatile uint32_t cpuvar;
} cpudata_t;

// --- State
//
// The ADS131M08 read lives on PRU0 (the only real-time core on AM335x). Channels
// are logged as raw sign-extended 24-bit codes -- scale to volts on the host --
// mirroring apps/test-ads131m08 on the am64x branch.
typedef struct {
  volatile uint32_t frame;        // monotonic loop counter (PRU0 writes pru_count_t.frame)
  volatile int32_t  adc[8];       // 8 channel conversions (sign-extended 24-bit)
  volatile uint32_t response;     // ADS131 STATUS/response word (per frame)
  volatile uint32_t id;           // ID register read at init (0x28xx expected)
  volatile uint32_t sync_state;   // external sync line (PRU1)
  cpudata_t cpudata;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;


#endif
