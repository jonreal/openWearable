#ifndef R5F_RPROC_H
#define R5F_RPROC_H
#include <stdint.h>
#ifndef R5F_CORE
#error "define R5F_CORE (0 or 1)"
#endif
// main_r5fss0 mailbox = main-navss cluster1 @ 0x31f81000 (OMAP map). core0: Linux
// tx=fifo1 / rx=fifo0; core1: tx=fifo3 / rx=fifo2.
#define MBOX1_BASE        0x31f81000u
#define MBOX_MESSAGE(m)   (MBOX1_BASE + 0x40u + 4u*(m))
#define MBOX_MSGSTATUS(m) (MBOX1_BASE + 0xC0u + 4u*(m))
#if R5F_CORE == 0
  #define R5F_RX_FIFO 1u
  #define R5F_TX_FIFO 0u
#else
  #define R5F_RX_FIFO 3u
  #define R5F_TX_FIFO 2u
#endif
#define RP_MBOX_SHUTDOWN     0xFFFFFF14u
#define RP_MBOX_SHUTDOWN_ACK 0xFFFFFF15u
#define R5F_RD32(a)   (*(volatile uint32_t*)(uintptr_t)(a))
#define R5F_WR32(a,v) (*(volatile uint32_t*)(uintptr_t)(a) = (uint32_t)(v))

// 1 if the A72 issued a graceful stop (RP_MBOX_SHUTDOWN).
static inline int R5fShutdownRequested(void) {
  if (R5F_RD32(MBOX_MSGSTATUS(R5F_RX_FIFO)) != 0u)
    return (R5F_RD32(MBOX_MESSAGE(R5F_RX_FIFO)) == RP_MBOX_SHUTDOWN);
  return 0;
}
// Ack the stop and park in WFI; the Device Manager then resets us. Never returns.
static inline void R5fShutdownAckHalt(void) {
  R5F_WR32(MBOX_MESSAGE(R5F_TX_FIFO), RP_MBOX_SHUTDOWN_ACK);
  __asm__ volatile("dsb");
  __asm__ volatile("isb");
  for (;;) __asm__ volatile("wfi");
}
#endif
