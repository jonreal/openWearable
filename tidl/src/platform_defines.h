/* openWearable: platform_defines for the fleet-free C7x TIDL device RT build.
 * L4_MEM_SIZE MUST match s_l4[] in ow_c7x_tidl.c (the EXTMEM/DDR pool). */
#ifndef __PLATFORM_DEFINES_H__
#define __PLATFORM_DEFINES_H__
#include <stdio.h>
#define TI_FILE FILE
#define FOPEN fopen
#define FGETS fgets
#define FCLOSE fclose
#define FWRITE fwrite
#define FSEEK fseek
#define FTELL ftell
#define FREAD fread
#define FPRINTF fprintf
#define EXTRA_MEM_FOR_ALIGN (1024)
#define L4_MEM_SIZE  (24 * 1024 * 1024)
#endif
