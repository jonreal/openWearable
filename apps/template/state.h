#ifndef _STATE_
#define _STATE_

#include "fix16.h"

// packing can be issue -- use 32bit types


// --- NN inference IO (C7x + MMA TIDL) ---------------------------------------
// Model tensor sizes -- must match the compiled TIDL net and the host tooling
// (tidl/host/ow_c7x_infer.py N_IN/N_OUT).
#define N_FEAT 16   // model input  width
#define N_OUT  8    // model output width

// A72-private model input. Produced by NnForward() from a window over state[];
// it never lives in PRU shared RAM (the A72<->C7x mailbox carries it as float32).
typedef struct {
  float x[N_FEAT];
} nn_feat_t;

// NN result. Written by the A72 inference thread; PRU0 snapshots cpu_state into
// the state ring every tick (pru0_main.c) -> read-only sample-and-hold for PRU,
// exactly like params. Seqlock: `seq` is odd while y[] is mid-write, even when
// stable; a tear-free consumer reads seq (even), reads y[], re-reads seq (equal).
typedef struct {
  volatile fix16_t  y[N_OUT];   // prediction (Q16.16)
  volatile uint32_t seq;        // seqlock counter (even = stable, odd = writing)
  volatile uint32_t stamp;      // state.frame the input window ended on (staleness)
  volatile int32_t  status;     // last TIDLRT return code (0 = ok, <0 = host err)
} nn_out_t;

// --- cpu_state struct
typedef struct {
  volatile uint32_t cpuvar;
  nn_out_t nn;                  // NN result -- rides the existing cpu_state snapshot
} cpu_state_t;

// --- r5f_state: the R5F pair's output, nested in the snapshot like cpu_state.
// Written by the R5F into ICSSG shared RAM; PRU0 snapshots it into the state ring.
typedef struct {
  volatile uint32_t r5fvar;
} r5f_state_t;

// --- State
typedef struct {
  volatile uint32_t frame;        // monotonic loop counter (PRU0 writes pru_count_t.frame)
  volatile uint32_t pru0var;
  volatile uint32_t pru1var;
  cpu_state_t cpu_state;
  r5f_state_t r5f_state;
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
