/* openWearable R5F control firmware (lockstep). The single lockstep core reads the
 * PRU's shared state in ICSSG RAM (0x0b010000), decimates off the PRU `frame`, runs
 * the app R5f0* hooks, and writes its output into the live shared_mem.r5f_state --
 * PRU0 snapshots it into the ring (one log). The crt0 (r5f_start.S) has a fault
 * handler so an unexpected abort can't silently wedge the core. MPU off. */
#include <stdint.h>
#include "r5floop.h"
#include "r5f_rproc.h"
#include "rsc_table.h"

volatile uint32_t* r5f_debug_buff;

int main(void) {
  shared_mem_t* sm = (shared_mem_t*)(uintptr_t) global_sharedram_base;
  param_mem_t*  pm = (param_mem_t*)(uintptr_t)(global_sharedram_base + param_ram_offset);
  pru_mem_t     mem = { sm, pm, 0 };
  r5f_debug_buff = &pm->debug_buff[0];

  pru_count_t counter = { 0, 0 };
  R5f0Init(&mem);

  r5f_view_t view = { &counter, pm };
  r5f_io_t   io   = { &sm->r5f_state, &sm->pru_ctl };

  uint32_t decim = pm->r5f_decimate;   // A72-set; cached (static per run)
  if (decim == 0u) decim = 1u;
  uint32_t last = 0xFFFFFFFFu;
  for (;;) {
    if (R5fShutdownRequested()) R5fShutdownAckHalt();   // mailbox first -- always killable
    uint32_t f = sm->state[sm->cbuff_index].frame;       // the shared PRU clock
    if ((f % decim) == 0u && f != last) {
      last = f;
      counter.frame = f;
      R5f0UpdateState(&view, &io);
      R5f0UpdateControl(&view, &io);
      counter.index++;
    }
  }
  return 0;
}
