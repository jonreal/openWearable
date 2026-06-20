ENTRY(_vectors)
MEMORY {
  ATCM     (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00008000
  CARVEOUT (rw)  : ORIGIN = 0xa2100000, LENGTH = 0x00100000
}
_stack_top = ORIGIN(ATCM) + LENGTH(ATCM);
SECTIONS {
  .vectors 0x00000000 : { KEEP(*(.vectors)) } > ATCM
  .text : { *(.text.start) *(.text*) *(.rodata*) *(.glue_7*) } > ATCM
  .data : { *(.data*) } > ATCM
  .bss (NOLOAD) : { . = ALIGN(4); __bss_start = .; *(.bss*) *(COMMON); . = ALIGN(4); __bss_end = .; } > ATCM
  .resource_table 0xa2100000 : { KEEP(*(.resource_table)) } > CARVEOUT
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment) *(.note*) *(.init*) *(.fini*) }
}
