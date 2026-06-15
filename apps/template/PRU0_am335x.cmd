/* Modified Jonathan Realmuto (c) 2017 */

/****************************************************************************/
/*  AM335x_PRU.cmd                                                          */
/*  Copyright (c) 2014  Texas Instruments Incorporated                      */
/*                                                                          */
/*    Description: This file is a linker command file that can be used for  */
/*                 linking PRU programs built with the C compiler and       */
/*                 the resulting .out file on an AM335x device.             */
/****************************************************************************/

-cr               /* Link using C conventions */

/* Specify the System Memory Map */
MEMORY
{
      PAGE 0:
  PRU_IMEM    : org = 0x00000000 len = 0x00002000  /* 8kB PRU0 Instruction RAM */

      PAGE 1:

  /* RAM Allocation
   *
   * Local memory map (ie, pru dependent)
   *
   * Parameters : PRU0_RAM (1kB)
   * Lookup tables : PRU1_RAM (1kB)
   * DataRam : PRUn_RAM (7 kB) - n is which ever pru */

  PRU_PARAM     : org = 0x00000000 len = 0x00000400 /* 1kB Parameters   */
  PRU_LUTAB     : org = 0x00002000 len = 0x00000C00 /* 3kB lookup Table */
  PRU_RAM       : org = 0x00000400 len = 0x00001C00 /* 7kB PRU RAM      */
  PRU_SHAREDMEM : org = 0x00010000 len = 0x00003000 /* 12kB Shared RAM  */


  /* Alternative mem model w more RAM but no lookup table */
/*
  PRU_PARAM     : org = 0x00000000 len = 0x00000200 * 0.5kB Parameters    *
  PRU_LUTAB     : org = 0x00002000 len = 0x00000200 * 0.5kB lookup Table  *
  PRU_RAM       : org = 0x00000200 len = 0x00001C00 * 7.5kB PRU RAM       *
  PRU_SHAREDMEM : org = 0x00010000 len = 0x00003000 * 12kB Shared RAM     *
*/

  /* Peripherals */

  PRU_CFG     : org = 0x00026000 len = 0x00000100 CREGISTER=4
  PRU_IEP     : org = 0x0002E000 len = 0x0000031C CREGISTER=26
  PRU_INTC    : org = 0x00020000 len = 0x00001504 CREGISTER=0

  /*
  RSVD10      : org = 0x48318000 len = 0x00000100 CREGISTER=10
  RSVD13      : org = 0x48310000 len = 0x00000100 CREGISTER=13
  RSVD27      : org = 0x00032000 len = 0x00000100 CREGISTER=27
  */
}

/* Specify the sections allocation into memory */
SECTIONS {
  /* Forces _c_int00 to the start of PRU IRAM. Not necessary when loading
     an ELF file, but useful when loading a binary */
  .text:_c_int00* >  0x0, PAGE 0

  .text   >  PRU_IMEM, PAGE 0
  .stack    >  PRU_RAM, PAGE 1
  .bss    >  PRU_RAM, PAGE 1
  .cio    >  PRU_RAM, PAGE 1
  .data   >  PRU_RAM, PAGE 1
  .switch   >  PRU_RAM, PAGE 1
  .sysmem   >  PRU_RAM, PAGE 1
  .cinit    >  PRU_RAM, PAGE 1
  .rodata   >  PRU_RAM, PAGE 1
  .rofardata  >  PRU_RAM, PAGE 1
  .farbss   >  PRU_RAM, PAGE 1
  .fardata  >  PRU_RAM, PAGE 1

  .resource_table > PRU_RAM, PAGE 1

  .pru_irq_map (COPY) :
  {
     *(.pru_irq_map)
  }
}

/* Declare global symbols for C */
__PRU_PARAM     = 0x00000000;
__PRU_LUTAB     = 0x00002000;
__PRU_SHAREDMEM = 0x00010000;

