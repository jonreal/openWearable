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
  pru_ctl_t*   ctl;         // shared flow-control utility bits
} r5f_io_t;

// Per-app hooks. r5f0 = main_r5fss0_core0 (5c00000), r5f1 = core1 (5d00000).
void R5f0Init(pru_mem_t* mem);
void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io);
void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io);
void R5f0Cleanup(void);
void R5f1Init(pru_mem_t* mem);
void R5f1UpdateState(const r5f_view_t* view, r5f_io_t* io);
void R5f1UpdateControl(const r5f_view_t* view, r5f_io_t* io);
void R5f1Cleanup(void);

// Flow-control utility bits (the shared pru_ctl; mirrors PruGetCtlBit etc.).
static inline uint8_t R5fGetCtlBit(const pru_ctl_t* c, uint8_t n) { return ((c->bit.utility & (1u << n)) == (1u << n)); }
static inline void R5fSetCtlBit(pru_ctl_t* c, uint8_t n) { c->bit.utility |= (1u << n); }
static inline void R5fClearCtlBit(pru_ctl_t* c, uint8_t n) { c->bit.utility &= ~(1u << n); }
#endif
