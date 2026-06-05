/* Modified by Jonathan Realmuto (c) 2017 */

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

	/* 12 KB PRU Instruction RAM */
  PRU_IMEM    : org = 0x00000000 len = 0x00003000 

      PAGE 1:

  /* Data RAM  16 kB */
  PRU_RAM       : org = 0x00000000 len = 0x00004000 CREGISTER=24

      PAGE 2:

	/* 64 KB PRU Shared RAM
   * 60 kB Shared Memory
   * 1 KB Parameters
   * 3 kB LUT */

  /* 60KB shared state */
  PRU_SHAREDMEM   : org = 0x00010000 len = 0x0000F000 CREGISTER=28

  /* 1KB parameters */
  PRU_PARAM       : org = 0x0001F000 len = 0x00000400 CREGISTER=29

  /* 3 kB LUT */
  PRU_LUTAB       : org = 0x0001F400 len = 0x00000C00 CREGISTER=30

  /* Internal Peripherals */
	/* NOTE: Use full INTC length instead of 0x200 to match the pruIntc
	 * structure definition in pru_intc.h, ignoring the second Constant
	 * Register #6 that starts at 0x200 offset within INTC */
	PRU_INTC	: org = 0x00020000 len = 0x00001504	CREGISTER=0
	PRU_IEP0	: org = 0x0002E000 len = 0x00000100	CREGISTER=26
	PRU_IEP0_0x100	: org = 0x0002E100 len = 0x0000021C	CREGISTER=8
	PRU_ECAP	: org = 0x00030000 len = 0x00000060	CREGISTER=3
	PRU_CFG		: org = 0x00026000 len = 0x00000100	CREGISTER=4
	PRU_CFG_0x100	: org = 0x00026100 len = 0x00000098	CREGISTER=5

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
}

