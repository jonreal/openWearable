/* openWearable R5F "park" firmware -- the default boot stub. Does nothing but
 * honor the K3 remoteproc graceful-stop handshake (RP_MBOX_SHUTDOWN -> ACK ->
 * WFI) so the A72 can stop it and load OW control firmware at runtime. MPU off
 * (the R5F reaches the mailbox + DDR carveout coherently with no MPU setup).
 * Built per core via -DR5F_CORE (selects mailbox FIFOs + carveout, ow_r5f.h). */
#include "ow_r5f.h"
#include "rsc_table.h"
#define OW_MAGIC 0x5F5FA720u
int main(void) {
    WR32(OW_DBG_BASE + 0x00, OW_MAGIC);   /* alive marker            */
    WR32(OW_DBG_BASE + 0x04, 0u);         /* heartbeat               */
    WR32(OW_DBG_BASE + 0x08, 0u);         /* last mailbox msg read   */
    WR32(OW_DBG_BASE + 0x0C, 0u);         /* last MSGSTATUS(RX) seen */
    uint32_t beat = 0u;
    for (;;) {
        WR32(OW_DBG_BASE + 0x04, ++beat);
        uint32_t st = RD32(MBOX_MSGSTATUS(RX_FIFO));
        WR32(OW_DBG_BASE + 0x0C, st);
        if (st != 0u) {
            uint32_t msg = RD32(MBOX_MESSAGE(RX_FIFO));
            WR32(OW_DBG_BASE + 0x08, msg);
            if (msg == RP_MBOX_SHUTDOWN) {
                WR32(MBOX_MESSAGE(TX_FIFO), RP_MBOX_SHUTDOWN_ACK);
                __asm__ volatile("dsb");
                __asm__ volatile("isb");
                for (;;) __asm__ volatile("wfi");
            }
        }
    }
    return 0;
}
