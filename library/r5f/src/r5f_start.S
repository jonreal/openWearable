  .syntax unified
  .arm
  .section .vectors, "ax"
  .global _vectors
_vectors:
  b _start
  b .
  b .
  b _pabort
  b _dabort
  b .
  b .
  b .

  .section .text.start, "ax"
  .global _start
  .type _start, %function
_start:
  ldr   sp, =_stack_top
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

  @ Fault handlers: stamp the cause/address to the carveout dbg window
  @ (_dbg_base = CARVEOUT + 0x400) so the A72 can read why a core wedged.
  .type _dabort, %function
_dabort:
  ldr   r0, =_dbg_base
  ldr   r1, =0xDEAD0DAB
  str   r1, [r0, #16]
  mrc   p15, 0, r1, c6, c0, 0    @ DFAR
  str   r1, [r0, #20]
  mrc   p15, 0, r1, c5, c0, 0    @ DFSR
  str   r1, [r0, #24]
  dsb
3:
  b     3b
  .type _pabort, %function
_pabort:
  ldr   r0, =_dbg_base
  ldr   r1, =0xDEAD0FAB
  str   r1, [r0, #16]
  mrc   p15, 0, r1, c6, c0, 2    @ IFAR
  str   r1, [r0, #20]
  mrc   p15, 0, r1, c5, c0, 1    @ IFSR
  str   r1, [r0, #24]
  dsb
4:
  b     4b
