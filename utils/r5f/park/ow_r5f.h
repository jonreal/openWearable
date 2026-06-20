#ifndef OW_R5F_H
#define OW_R5F_H
#include <stdint.h>
#ifndef R5F_CORE
#error "define R5F_CORE (0 = main_r5fss0_core0 / 5c00000, 1 = core1 / 5d00000)"
#endif
/* main_r5fss0 mailbox = main-navss cluster1 @ 0x31f81000 (DT mboxes =
 * <&mailbox0_cluster1 &mbox_main_r5fss0_coreX>). OMAP register map:
 *   MESSAGE(m)   = base + 0x40 + 4*m   (read = dequeue, write = enqueue)
 *   MSGSTATUS(m) = base + 0xC0 + 4*m   (pending message count) */
#define MBOX1_BASE        0x31f81000u
#define MBOX_MESSAGE(m)   (MBOX1_BASE + 0x40u + 4u*(m))
#define MBOX_MSGSTATUS(m) (MBOX1_BASE + 0xC0u + 4u*(m))
#if R5F_CORE == 0
  #define RX_FIFO 1u                  /* Linux tx -> R5F reads SHUTDOWN */
  #define TX_FIFO 0u                  /* Linux rx <- R5F writes ACK     */
  #define CARVEOUT_BASE 0xa2100000u
#elif R5F_CORE == 1
  #define RX_FIFO 3u
  #define TX_FIFO 2u
  #define CARVEOUT_BASE 0xa4100000u
#else
  #error "R5F_CORE must be 0 or 1"
#endif
/* liveness/debug window past the resource table (A72 reads via /dev/mem O_SYNC). */
#define OW_DBG_BASE  (CARVEOUT_BASE + 0x400u)
/* K3 remoteproc graceful-shutdown handshake (ti_k3_r5_remoteproc.ko, HW-verified). */
#define RP_MBOX_SHUTDOWN     0xFFFFFF14u
#define RP_MBOX_SHUTDOWN_ACK 0xFFFFFF15u
#define RD32(a)   (*(volatile uint32_t*)(uintptr_t)(a))
#define WR32(a,v) (*(volatile uint32_t*)(uintptr_t)(a) = (uint32_t)(v))
#endif
