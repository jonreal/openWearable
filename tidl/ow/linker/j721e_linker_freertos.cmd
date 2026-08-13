/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
--ram_model
-heap  0x20000
-stack 0x20000
--args 0x1000
--diag_suppress=10068 /* "no matching section" */
--cinit_compression=off
-e _c_int00_secure

SECTIONS
{
    boot:
    {
      boot.*<boot.oe71>(.text)
    } load > DDR_C7x_1_BOOT ALIGN(0x200000)
    .vecs       >       DDR_C7x_1_VECS ALIGN(0x400000)
    .secure_vecs >      DDR_C7x_1_SECURE_VECS ALIGN(0x200000)
    .text:_c_int00_secure > DDR_C7x_1_BOOT ALIGN(0x200000)
    .text       >       DDR_C7x_1 ALIGN(0x200000)

    .bss        >       DDR_C7x_1  /* Zero-initialized data */
    .data       >       DDR_C7x_1  /* Initialized data */

    .cinit      >       DDR_C7x_1  /* could be part of const */
    .init_array >       DDR_C7x_1  /* C++ initializations */
    .stack      >       DDR_C7x_1  ALIGN(0x20000) /* MUST be 128KB aligned to handle nested interrupts */
    .args       >       DDR_C7x_1
    .cio        >       DDR_C7x_1
    .const      >       DDR_C7x_1
    .switch     >       DDR_C7x_1
    .sysmem     >       DDR_C7x_1 /* heap */
    /* .bss:taskStackSection:tiovx (NOLOAD) : {} > L2RAM_C7x_1 */
    .bss:taskStackSection       > DDR_C7x_1
    .bss:ddr_local_mem      (NOLOAD) : {} > DDR_C7X_1_LOCAL_HEAP
    .bss:ddr_scratch_mem    (NOLOAD) : {} > DDR_C7X_1_SCRATCH

    .bss:app_log_mem        (NOLOAD) : {} > APP_LOG_MEM
    .bss:app_fileio_mem     (NOLOAD) : {} > APP_FILEIO_MEM
    .bss:tiovx_obj_desc_mem (NOLOAD) : {} > TIOVX_OBJ_DESC_MEM
    .bss:ipc_vring_mem      (NOLOAD) : {} > IPC_VRING_MEM

    .bss:l1mem              (NOLOAD)(NOINIT) : {} > L1RAM_C7x_1
    .bss:l2mem              (NOLOAD)(NOINIT) : {} > L2RAM_C7x_1
    .bss:l3mem              (NOLOAD)(NOINIT) : {} > MSMC_C7x_1

    ipc_data_buffer:       > DDR_C7x_1
    .tracebuf                : {} align(1024)   > DDR_C7x_1
    .resource_table > DDR_C7x_1_RESOURCE_TABLE

    GROUP:              >  DDR_C7x_1
    {
        .data.Mmu_tableArray          : type=NOINIT
        .data.Mmu_tableArraySlot      : type=NOINIT
        .data.Mmu_level1Table         : type=NOINIT
        .data.Mmu_tableArray_NS       : type=NOINIT
        .data.Mmu_tableArraySlot_NS   : type=NOINIT
        .data.Mmu_level1Table_NS      : type=NOINIT
    }


}
