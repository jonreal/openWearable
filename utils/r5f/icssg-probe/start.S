  .syntax unified
  .arm
  .section .vectors, "ax"
  .global _vectors
_vectors:
  b _start          /* reset */
  b .               /* undef */
  b .               /* svc */
  b .               /* prefetch abort */
  b _data_abort     /* data abort  <-- a firewall block lands here */
  b .               /* reserved */
  b .               /* irq */
  b .               /* fiq */

  .section .text.start, "ax"
  .global _start
  .type _start, %function
_start:
  ldr   sp, =_stack_top
  cpsie a                     /* unmask async (external) aborts so a firewall traps */
  ldr   r0, =__bss_start
  ldr   r1, =__bss_end
  movs  r2, #0
0:
  cmp   r0, r1
  bcs   1f
  str   r2, [r0], #4
  b     0b
1:
  bl    main
2:
  b     2b

/* Data-abort handler: record the fault into the (known-good) R5F carveout at
 * 0xa2100410 and halt, so the A72 can tell "firewall blocked it" from "it worked". */
  .global _data_abort
  .type _data_abort, %function
_data_abort:
  movw  r0, #0x0410
  movt  r0, #0xa210           /* r0 = 0xa2100410 */
  mrc   p15, 0, r1, c6, c0, 0 /* DFAR (fault address) */
  mrc   p15, 0, r2, c5, c0, 0 /* DFSR (fault status) */
  movw  r3, #0xDEAD
  str   r3, [r0]
  str   r1, [r0, #4]
  str   r2, [r0, #8]
  dsb
3:
  b     3b
