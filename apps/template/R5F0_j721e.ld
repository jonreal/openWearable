/* GNU ld linker script for the lockstep R5F firmware (arm-none-eabi-gcc).
 * (clpru consumes a .cmd command-file; GNU ld consumes a .ld linker script --
 *  same role, different toolchain.) Code/data/stack live in ATCM (on-core, 32 KB,
 *  safe to execute with MPU off); only the resource table sits in the DDR carveout
 *  (main_r5fss0_core0 @ a2100000). _dbg_base = carveout+0x400 is the fault/debug
 *  window the crt0 stamps (see r5f_start.S). */
ENTRY(_vectors)
MEMORY {
  ATCM     (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00008000
  CARVEOUT (rw)  : ORIGIN = 0xa2100000, LENGTH = 0x00100000
}
_stack_top = ORIGIN(ATCM) + LENGTH(ATCM);
_dbg_base  = 0xa2100000 + 0x400;
SECTIONS {
  .vectors 0x00000000 : { KEEP(*(.vectors)) } > ATCM
  .text : { *(.text.start) *(.text*) *(.rodata*) *(.glue_7*) } > ATCM
  .data : { *(.data*) } > ATCM
  .bss (NOLOAD) : { . = ALIGN(4); __bss_start = .; *(.bss*) *(COMMON); . = ALIGN(4); __bss_end = .; } > ATCM
  .resource_table 0xa2100000 : { KEEP(*(.resource_table)) } > CARVEOUT
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment) *(.note*) *(.init*) *(.fini*) }
}
