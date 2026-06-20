/* openWearable R5F <-> ICSSG shared-RAM access probe.
 *
 * Gating test for the UNIFIED PRU+R5F memory model: can the R5F (main domain)
 * read/write the PRU's ICSSG shared RAM at 0x0b010000? Runs on core0; reports to
 * the core0 DDR carveout (0xa2100400, known accessible). A firewall block traps
 * to the data-abort handler (start.S), which stamps 0xDEAD + the fault regs.
 *
 * Read with ./read.py. A meaningful test needs the ICSSG clocked + (ideally) a
 * PRU writing its debug counter -> run the template app first (see README). */
#include <stdint.h>
#include "rsc_table.h"

#define RD32(a)   (*(volatile uint32_t*)(uintptr_t)(a))
#define WR32(a,v) (*(volatile uint32_t*)(uintptr_t)(a) = (uint32_t)(v))

#define DBG            0xa2100400u          /* R5F own carveout (baseline-good)   */
#define ICSSG_BASE     0x0b010000u          /* PRU shared RAM (the thing tested)  */
#define ICSSG_SCRATCH (ICSSG_BASE + 0x2000u) /* unused gap in shared_mem_t        */
#define ICSSG_PRU_DBG (ICSSG_BASE + 0xE000u) /* param.debug_buff[0]: PRU0 ticks   */

/* core0 mailbox (cluster1 fifo1=rx / fifo0=tx) for the rproc stop handshake */
#define MBOX_MSG(m)  (0x31f81000u + 0x40u + 4u*(m))
#define MBOX_STAT(m) (0x31f81000u + 0xC0u + 4u*(m))

int main(void) {
  WR32(DBG + 0x10, 0u);               /* clear abort marker        */
  WR32(DBG + 0x00, 0x5F5F0B10u);      /* probe alive               */
  uint32_t beat = 0u, pat = 0xC0FFEE00u;

  for (;;) {
    WR32(DBG + 0x04, ++beat);

    /* (1) write+read-back ICSSG scratch -- a firewall block aborts here */
    WR32(ICSSG_SCRATCH, pat);
    WR32(DBG + 0x08, RD32(ICSSG_SCRATCH));   /* == pat if R/W works */

    /* (2) read the PRU's live debug counter (climbs when a PRU app runs) */
    WR32(DBG + 0x0C, RD32(ICSSG_PRU_DBG));
    pat++;

    /* rproc graceful stop */
    if (RD32(MBOX_STAT(1u)) != 0u && RD32(MBOX_MSG(1u)) == 0xFFFFFF14u) {
      WR32(MBOX_MSG(0u), 0xFFFFFF15u);
      __asm__ volatile("dsb"); __asm__ volatile("isb");
      for (;;) __asm__ volatile("wfi");
    }
  }
  return 0;
}
