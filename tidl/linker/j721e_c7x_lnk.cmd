/* openWearable lean C7x TIDL firmware — linker command file (J721E / TDA4VM)
 *
 * Derived from TI PSDK vision_apps j721e c7x_1 `linker_mem_map.cmd` (MEMORY) +
 * `j721e_linker_freertos.cmd` (SECTIONS). Kept close to the proven vision_apps layout so the
 * Linux K3 remoteproc loader maps it identically.
 *
 * The MEMORY map MUST stay in sync with the C7x carveouts in
 * device-tree/k3-j721e-boneai64-openWearable-tidl.dts.
 */
--ram_model
-heap   0x20000
-stack  0x20000
--args  0x1000
--diag_suppress=10068      /* "no matching section" */
--cinit_compression=off
-e _c_int00_secure

MEMORY
{
    /* L2 for C7x_1 [ 448 KB ] */
    L2RAM_C7x_1              ( RWIX ) : ORIGIN = 0x64800000 , LENGTH = 0x00070000
    /* L1 for C7x_1 [ 16 KB ] */
    L1RAM_C7x_1              ( RWIX ) : ORIGIN = 0x64E00000 , LENGTH = 0x00004000
    /* MSMC for C7x_1 [ 7.78 MB ] */
    MSMC_C7x_1               ( RWIX ) : ORIGIN = 0x70020000 , LENGTH = 0x007C8000
    /* IPC Vrings (unused here; reserved to match carveout) [ 32 MB ] */
    IPC_VRING_MEM                     : ORIGIN = 0xAA000000 , LENGTH = 0x02000000
    /* Remote-core log — NON-CACHEABLE → A72 /dev/mem readout [ 256 KB ] */
    APP_LOG_MEM                       : ORIGIN = 0xAC000000 , LENGTH = 0x00040000
    /* TIOVX shared mem (unused) [ 63.75 MB ] */
    TIOVX_OBJ_DESC_MEM                : ORIGIN = 0xAC040000 , LENGTH = 0x03FC0000
    /* Remote-core file I/O (unused) [ 4 MB ] */
    APP_FILEIO_MEM                    : ORIGIN = 0xB0000000 , LENGTH = 0x00400000
    /* DDR for Linux IPC (unused) [ 1 MB ] */
    DDR_C7x_1_IPC            ( RWIX ) : ORIGIN = 0xB2000000 , LENGTH = 0x00100000
    /* Resource table [ 1 KB ] */
    DDR_C7x_1_RESOURCE_TABLE ( RWIX ) : ORIGIN = 0xB2100000 , LENGTH = 0x00000400
    /* Boot section [ 1 KB ] */
    DDR_C7x_1_BOOT           ( RWIX ) : ORIGIN = 0xB2200000 , LENGTH = 0x00000400
    /* Vecs [ 16 KB ] */
    DDR_C7x_1_VECS           ( RWIX ) : ORIGIN = 0xB2400000 , LENGTH = 0x00004000
    /* Secure vecs [ 16 KB ] */
    DDR_C7x_1_SECURE_VECS    ( RWIX ) : ORIGIN = 0xB2600000 , LENGTH = 0x00004000
    /* Code/data [ 89.98 MB ] */
    DDR_C7x_1                ( RWIX ) : ORIGIN = 0xB2604000 , LENGTH = 0x059FC000
    /* Shared DDR (dma-heap / tensor buffers) [ 512 MB ] */
    DDR_SHARED_MEM                    : ORIGIN = 0xB8000000 , LENGTH = 0x20000000
    /* Scratch (high DDR) [ 368 MB ] */
    DDR_C7X_1_SCRATCH        ( RWIX ) : ORIGIN = 0x100000000 , LENGTH = 0x17000000
    /* Local heap (high DDR) [ 256 MB ] */
    DDR_C7X_1_LOCAL_HEAP     ( RWIX ) : ORIGIN = 0x117000000 , LENGTH = 0x10000000
}

SECTIONS
{
    boot:
    {
      boot.*<boot.oe71>(.text)
    } load > DDR_C7x_1_BOOT ALIGN(0x200000)
    .vecs        >      DDR_C7x_1_VECS ALIGN(0x400000)
    .secure_vecs >      DDR_C7x_1_SECURE_VECS ALIGN(0x200000)
    .text:_c_int00_secure > DDR_C7x_1_BOOT ALIGN(0x200000)
    .text        >      DDR_C7x_1 ALIGN(0x200000)

    .bss         >      DDR_C7x_1
    .data        >      DDR_C7x_1
    .cinit       >      DDR_C7x_1
    .init_array  >      DDR_C7x_1
    .stack       >      DDR_C7x_1  ALIGN(0x20000)  /* 128KB aligned (nested IRQ) */
    .args        >      DDR_C7x_1
    .cio         >      DDR_C7x_1
    .const       >      DDR_C7x_1
    .switch      >      DDR_C7x_1
    .sysmem      >      DDR_C7x_1  /* heap */

    .bss:ddr_local_mem      (NOLOAD) : {} > DDR_C7X_1_LOCAL_HEAP
    .bss:ddr_scratch_mem    (NOLOAD) : {} > DDR_C7X_1_SCRATCH
    .bss:app_log_mem        (NOLOAD) : {} > APP_LOG_MEM
    .bss:l1mem              (NOLOAD)(NOINIT) : {} > L1RAM_C7x_1
    .bss:l2mem              (NOLOAD)(NOINIT) : {} > L2RAM_C7x_1
    .bss:l3mem              (NOLOAD)(NOINIT) : {} > MSMC_C7x_1

    .tracebuf                : {} align(1024) > DDR_C7x_1
    .resource_table          : {}            > DDR_C7x_1_RESOURCE_TABLE

    GROUP:               >  DDR_C7x_1
    {
        .data.Mmu_tableArray          : type=NOINIT
        .data.Mmu_tableArraySlot      : type=NOINIT
        .data.Mmu_level1Table         : type=NOINIT
        .data.Mmu_tableArray_NS       : type=NOINIT
        .data.Mmu_tableArraySlot_NS   : type=NOINIT
        .data.Mmu_level1Table_NS      : type=NOINIT
    }
}
