/* openWearable R5F core1 control firmware -- UNIFIED model. Same shape as
 * r5f0_main: decimates off the same PRU `frame` (so both cores stay synchronized
 * with no handshake), runs the app R5f1* hooks, writes its slice of r5f_state. */
#include <stdint.h>
#include "r5floop.h"
#include "r5f_rproc.h"
#include "rsc_table.h"

volatile uint32_t* r5f_debug_buff;
extern volatile uint32_t _dbg_base[];   // carveout dbg window (linker: CARVEOUT+0x400)

#define R5F_DECIM 10u

int main(void) {
  shared_mem_t* sm = (shared_mem_t*)(uintptr_t) global_sharedram_base;
  param_mem_t*  pm = (param_mem_t*)(uintptr_t)(global_sharedram_base + param_ram_offset);
  pru_mem_t     mem = { sm, pm, 0 };
  r5f_debug_buff = &pm->debug_buff[0];
  _dbg_base[0] = 0x5F5FA720u; _dbg_base[1] = 1u;   // DBG alive + stage1 (entered main)

  pru_count_t counter = { 0, 0 };
  R5f1Init(&mem);

  r5f_view_t view = { &counter, pm };
  r5f_io_t   io   = { &sm->r5f_state, &sm->pru_ctl };

  _dbg_base[1] = 2u;   // DBG stage2 (setup done)
  uint32_t beat = 0u;
  uint32_t last = 0xFFFFFFFFu;
  for (;;) {
    _dbg_base[2] = ++beat;   // DBG heartbeat
    if (R5fShutdownRequested()) R5fShutdownAckHalt();   // mailbox first -- stays killable even if ICSSG stalls
    _dbg_base[1] = 3u;   // DBG stage3 (about to read ICSSG)
    uint32_t f = sm->state[sm->cbuff_index].frame;
    _dbg_base[1] = 4u; _dbg_base[3] = f;   // DBG stage4 (ICSSG read OK)
    if ((f % R5F_DECIM) == 0u && f != last) {
      last = f;
      counter.frame = f;
      R5f1UpdateState(&view, &io);
      R5f1UpdateControl(&view, &io);
      counter.index++;
    }
  }
  return 0;
}
