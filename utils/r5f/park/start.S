  .syntax unified
  .arm
  .section .vectors, "ax"
  .global _vectors
_vectors:
  b   _start
  b   .
  b   .
  b   .
  b   .
  b   .
  b   .
  b   .
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
