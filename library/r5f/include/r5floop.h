#ifndef _R5FLOOP_H_
#define _R5FLOOP_H_

#include "mem_types.h"   // unified contract: state_t / shared_mem_t / pru_ctl_t / r5f_state_t / pru_mem_t

// Per-core debug buffer (like the PRU's debug_buff).
extern volatile uint32_t* r5f_debug_buff;

// Hook context (same split as the PRU): a const `view` of the static params + a
// mutable `io` surface this tick. The R5F main loop owns the frame-count decimation
// and the rproc stop-handshake -- the app writes only the hooks. The R5F firmware
// itself, like the A72, has full access to the shared layout; `io` is just the
// curated hook surface, widened as hooks come to need more.
typedef struct {
  const pru_count_t* c;     // decimation counters (frame / tick)
  const param_mem_t* p;     // static params
} r5f_view_t;

typedef struct {
  r5f_state_t* s;           // this pair's live output (shared_mem.r5f_state); PRU snapshots it
  pru_ctl_t*   ctl;         // PRU flow-control word (a hook may read enable/done to gate)
} r5f_io_t;

// Per-app hooks (one lockstep core). r5f0 = main_r5fss0 (active core = core0, 5c00000).
void R5f0Init(pru_mem_t* mem);
void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io);
void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io);
void R5f0Cleanup(void);

#endif
