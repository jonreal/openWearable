/* openWearable — fleet-free C7x TIDL inference driver  (Path-Y step 2)
 *
 * Runs a TIDL network on the C7x via the device-side TIDLRT API (NON-TIOVX), inside the
 * vision_apps boot shell with appInit()/appRun() removed (see main.c appMain). This file is
 * the *platform layer* that TI's device RT harness (ti_dl/test/src/tidl_rt.c) expects a caller
 * to provide: the scratch memory pools (L1..L4), the UDMA driver handle (gUdmaDrvObjPtr), and a
 * handful of test-bench helper symbols that tidl_rt.c references. We deliberately do NOT compile
 * tidl_tb.c / tidl_tb_utils.c (file I/O test harness); we satisfy tidl_rt.c's externals here.
 *
 * Build (concerto_c7x_1_inc.mak): compiles alongside tidl_rt.c + ti_mem_manager.c with TIDL
 * IDIRS. The TIDL engine libs (tidl_algo/priv/obj/custom, mmalib, common_C7100) + UDMA/sciclient
 * are already linked by the c7x app — they were only dead-stripped in step 1 because nothing
 * referenced them.
 *
 * Transport = DDR mailbox in DDR_SHARED_MEM (0xB8000000, mapped by main.c appMmuMap): the A72
 * host tool (host/ow_c7x_infer.py) writes net.bin/io.bin/input + a READY flag; this firmware
 * spin-waits, runs inference, writes float output + a DONE flag; the A72 reads + compares.
 * DDR_SHARED is cacheable on C7x, so we CacheP_Inv before reads / CacheP_wb after writes; the
 * A72 maps /dev/mem O_SYNC (uncached) so it sees coherent DRAM.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <ti/osal/CacheP.h>
#include "itidl_ti.h"          /* TIDL_* element-type enums, sTIDL_Network_t            */
#include "itidl_rt.h"          /* sTIDLRT_Params_t, sTIDLRT_Tensor_t, TIDLRT_*          */
#include "ivision.h"           /* IVISION_BufDesc (stub signatures)                     */

#include <utils/console_io/include/app_log.h>     /* appLogPrintf                       */
#include <utils/ipc/include/app_ipc.h>            /* APP_IPC_CPU_C7x_1                   */
#include <utils/udma/include/app_udma.h>          /* appUdmaInit / appUdmaGetObj        */

extern int32_t appSciclientInit(uint32_t self_cpu_id);

/* ---- DDR mailbox layout (MUST match host/ow_c7x_infer.py) --------------------------- */
#define OW_MB_BASE        0xB8000000u
#define OW_MB_READY       (OW_MB_BASE + 0x00)   /* A72 -> C7x: 0xC7C7AA01 when inputs ready */
#define OW_MB_DONE        (OW_MB_BASE + 0x10)   /* C7x -> A72: 0xC7C7DD01 when output ready */
#define OW_MB_STATUS      (OW_MB_BASE + 0x20)   /* C7x -> A72: TIDLRT return code           */
#define OW_MB_NETLEN      (OW_MB_BASE + 0x30)   /* A72 -> C7x: net.bin byte length          */
#define OW_MB_IOLEN       (OW_MB_BASE + 0x38)   /* A72 -> C7x: io.bin  byte length          */
#define OW_MB_INVOKES     (OW_MB_BASE + 0x70)   /* C7x -> A72: cumulative invoke count (loop) */
#define OW_MB_NET         (OW_MB_BASE + 0x00100000)  /* net.bin (sTIDL_Network_t)           */
#define OW_MB_IO          (OW_MB_BASE + 0x02000000)  /* io.bin  (sTIDL_IOBufDesc_t)         */
#define OW_MB_INPUT       (OW_MB_BASE + 0x02100000)  /* input  float32[OW_N_IN]             */
#define OW_MB_OUTPUT      (OW_MB_BASE + 0x02180000)  /* output float32[OW_N_OUT]            */

#define OW_MB_READY_MAGIC  0xC7C7AA01u
#define OW_MB_DONE_MAGIC   0xC7C7DD01u

#define OW_N_IN   16
#define OW_N_OUT   8

#define RD32(a)   (*(volatile uint32_t *)(uintptr_t)(a))
#define WR32(a,v) (*(volatile uint32_t *)(uintptr_t)(a) = (uint32_t)(v))

/* =====================================================================================
 *  Platform-layer symbols that ti_dl/test/src/tidl_rt.c references as externs.
 * ===================================================================================== */

/* tidl_rt.c globals (extern there, defined here). */
int   gWriteStaticBins = 0;
int   currentNetwork   = 0;
void *gPrivArgs        = NULL;
void *gUdmaDrvObjPtr   = NULL;       /* set from appUdmaGetObj() in ow_c7x_tidl_run()    */

/* tidl_rt.c references gParams.numFrames on the (never-taken) calibration branch. We avoid
 * pulling in tidl_config.h/configparser.h by providing a link-compatible sized blob; the
 * symbol just needs to exist and be large enough for the field offset. */
uint8_t gParams[8192];

/* Static-comparison reference arrays (only address-taken on the comparison path, which we
 * disable via tidl_is_comparison_required()==0). Small defs are sufficient to link. */
uint8_t tidl_tb_output_tensor[4096];
uint8_t tidl_tb_pOutx_tensor[4096];

/* TIDL scratch pools. L2 (DARAM1) + L3/MSMC (SARAM) MUST be on-chip SRAM, not DDR: the engine
 * sets cacheWriteBack=NULL and assumes the DRU sees CPU-written transfer records coherently.
 * Cacheable-DDR scratch -> the DRU reads stale TRs -> algProcess hangs. So point L2/L3 at the
 * real SRAM (MMU-mapped by main.c: L2 @0x64800000/448K, MSMC @0x70020000/~7.97MB). L1 (DARAM0,
 * 16K) + L4 (EXTMEM/DDR working) stay in DDR .bss. */
#define OW_ALIGN __attribute__((aligned(4096)))
static uint8_t s_l1[  32u * 1024u]            OW_ALIGN;   /* L1 scratch (DARAM0) */
static uint8_t s_l4[  24u * 1024u * 1024u]    OW_ALIGN;   /* DDR working (EXTMEM) */
uint8_t *L1Scratch = s_l1;
uint8_t *L2Scratch = (uint8_t *)0x64800000u;   /* C7x L2 SRAM (448 KB)   */
uint8_t *L3Scratch = (uint8_t *)0x70020000u;   /* MSMC SRAM   (~7.97 MB) */
uint8_t *L4Scratch = s_l4;

/* ---- engine diagnostics -> mailbox text ring (NOT appLogPrintf, which blocks deep inside
 *      TIDLRT_create without appInit's log writer and masks the real return). The A72 reads
 *      this text from the non-cacheable mailbox. -------------------------------------------- */
#define OW_LOG_BASE  (OW_MB_BASE + 0x200u)     /* 2 KB text region, host zeroes it pre-run */
#define OW_LOG_CAP   0x7FEu
static uint32_t ow_log_off = 0;
static void ow_log_str(const char *s)
{
    volatile char *dst = (volatile char *)(uintptr_t)OW_LOG_BASE;
    while (*s && ow_log_off < OW_LOG_CAP) dst[ow_log_off++] = *s++;
    dst[ow_log_off] = 0;
}

int32_t tidl_tb_utils_printf(const char *format, va_list args)
{
    char b[256];
    vsnprintf(b, sizeof(b), format, args);
    ow_log_str(b);
    return 0;
}

int tidl_tb_printf(int traceLevel, const char *format, ...)
{
    if (traceLevel <= 1)                 /* errors + key info; chatty levels dropped */
    {
        char b[256];
        va_list a;
        va_start(a, format);
        vsnprintf(b, sizeof(b), format, a);
        va_end(a);
        ow_log_str(b);
    }
    return 0;
}

/* ---- test-harness orchestration tidl_rt.c calls — no-op for a single embedded invoke -- */
void tidl_genSetStaticInput(sTIDL_IOBufDesc_t *ioPrms, IVISION_BufDesc *BufDescList)
{ (void)ioPrms; (void)BufDescList; }                 /* harmless: only acts if gWriteStaticBins */
void tidl_wait_for_uc_start(void) {}
void tidl_wait_for_uc_end(void)   {}
void tidl_write_frameHeader(int32_t frameIdx, int32_t codePosition)
{ (void)frameIdx; (void)codePosition; }
void tidl_write_frameInfo(int32_t frameIdx, uint64_t totalCycles, uint64_t ddrRead,
                          uint64_t ddrWrite, int32_t isMatching)
{ (void)frameIdx; (void)totalCycles; (void)ddrRead; (void)ddrWrite; (void)isMatching; }
int32_t tidl_is_comparison_required(void) { return 0; }   /* we compare on the A72 instead   */

/* ---- small leaf helpers (replicated from tidl_tb_utils.c) --------------------------- */
int32_t tidltb_getDatElementSize(int32_t elementType)
{
    if ((elementType == TIDL_SignedChar)  || (elementType == TIDL_UnsignedChar))  return 1;
    if ((elementType == TIDL_SignedShort) || (elementType == TIDL_UnsignedShort)) return 2;
    if ((elementType == TIDL_SinglePrecFloat) ||
        (elementType == TIDL_UnsignedWord) || (elementType == TIDL_SignedWord))   return 4;
    if ((elementType == TIDL_UnsignedDoubleWord) ||
        (elementType == TIDL_SignedDoubleWord))                                   return 8;
    return 1;
}

/* tidl_rt.c's TestApp_AllocMemRecords routes PERSISTENT external memrecs (the engine's
 * algorithm state, hundreds of KB) through this. The firmware RTS malloc heap is far too small
 * (~256 KB) -> NULL -> create fails. So bump-allocate from a large static DDR arena instead.
 * tidltb_alignFree is a no-op; the arena is reclaimed wholesale at the next create (ow_heap_off
 * reset there). */
#define OW_HEAP_SIZE  (32u * 1024u * 1024u)
static uint8_t  ow_heap[OW_HEAP_SIZE] OW_ALIGN;
uint32_t        ow_heap_off = 0;                 /* non-static: reset from ow_c7x_tidl_run() */
void *tidltb_alignMalloc(int32_t size, int32_t alignment)
{
    uint32_t a    = (alignment < 8) ? 8u : (uint32_t)alignment;
    uint32_t base = (ow_heap_off + a - 1u) & ~(a - 1u);
    if ((uint64_t)base + (uint32_t)size > OW_HEAP_SIZE) return NULL;   /* probe records the fail */
    ow_heap_off = base + (uint32_t)size;
    return &ow_heap[base];
}
void tidltb_alignFree(void *ptr) { (void)ptr; }

/* ---- more tidl_rt.c externals (the rest live in the un-compiled tidl_tb_utils.c) ----- */
void tidl_printStatus(int32_t status)
{
    char b[48];
    snprintf(b, sizeof(b), "tidl status=%d\n", (int)status);
    ow_log_str(b);
}
void tidl_tb_progressBar(float progress) { (void)progress; }
int32_t tidl_writeMultiNetworkBin(uint8_t *data_ptr, int8_t *FileName, int32_t data_size)
{ (void)data_ptr; (void)FileName; (void)data_size; return 0; }   /* file dump: not needed */
uint32_t TIDL_TEST_getCoreNum(void) { return 0; }                /* single-core C7x        */

/* Walk the net to find the output-data params for a given dataId (used by the input copy to
 * read tensorScale). Replicated verbatim from tidl_tb_utils.c. */
const sTIDL_DataParams_t *tidl_getDataBufDims(const sTIDL_Network_t *net, int32_t dataBuffId)
{
    int32_t i, j;
    for (i = 0; i < net->numLayers; i++)
        for (j = 0; j < net->TIDLLayers[i].numOutBufs; j++)
            if (net->TIDLLayers[i].outData.dataId == dataBuffId)
                return &net->TIDLLayers[i].outData;
    return NULL;
}

/* TI's TIDLRT_setParamsDefault lives in the A72 OVX runtime (tidl_rt_ovx.c), which we do not
 * compile. Replicate it for the device, dropping the A72 env-var flow-control lookups. */
int32_t TIDLRT_setParamsDefault(sTIDLRT_Params_t *prms)
{
    prms->netPtr                    = NULL;
    prms->ioBufDescPtr              = NULL;
    prms->net_capacity              = 0;
    prms->io_capacity               = 0;
    prms->flowCtrl                  = 0;
    prms->traceLogLevel             = 0;
    prms->traceWriteLevel           = 0;
    prms->dumpNetInitBackupData     = 0;
    strcpy(prms->traceBaseName, "/tmp/tidl_trace");
    prms->TIDLWriteBinToFile        = NULL;
    prms->TIDLReadBinFromFile       = NULL;
    prms->TIDLVprintf               = tidl_tb_utils_printf;
    prms->quantRangeExpansionFactor = 1;
    prms->quantRangeUpdateFactor    = 0;
    prms->stats                     = NULL;
    prms->maxPreEmptDelay           = FLT_MAX;
    prms->targetPriority            = 0;
    prms->coreNum                   = 1;
    prms->coreStartIdx              = 1;
    return 0;
}

/* =====================================================================================
 *  The inference entry point — called from main.c appMain (replaces the step-1 markers).
 * ===================================================================================== */
static float s_in[OW_N_IN];
static float s_out[OW_N_OUT];

/* Debug markers in the EXCLUSIVE mailbox scratch (0xB8000040+), not APP_LOG (which the other
 * fleet cores clobber). DDR_SHARED_MEM is mapped NON-cacheable by main.c appMmuMap, so plain
 * stores are coherent with the A72 /dev/mem (O_SYNC) view — no cache management needed. */
#define OW_DBG ((volatile uint32_t *)(uintptr_t)(OW_MB_BASE + 0x40))

int32_t ow_c7x_tidl_run(void)
{
    int32_t           status, rcSci, rcUdma;
    void             *handle = NULL;
    sTIDLRT_Params_t  prms;
    sTIDLRT_Tensor_t  inT, outT;
    sTIDLRT_Tensor_t *in[1]  = { &inT };
    sTIDLRT_Tensor_t *out[1] = { &outT };
    app_udma_init_prms_t udmaPrm;
    uint32_t          netLen, ioLen;
    uint32_t          nInvoke = 0;

    OW_DBG[0] = 0xABCD0001u;                       /* entered run */
    ow_log_str("ow_run_start\n");                   /* logging self-test */
    ow_heap_off = 0;                                /* reset the persistent-alloc arena */

    /* DDR_SHARED survives a warm reboot, so READY/DONE may still hold the previous run's magic.
     * Clear them up front so we always wait for a FRESH host kick (else we'd run create at boot
     * on stale data before the host stages anything). */
    WR32(OW_MB_READY, 0);
    WR32(OW_MB_DONE,  0);

    /* 1. Single-core inits the dropped appInit() used to do: sciclient (RM via TIFS) then UDMA.
     *    The TIDL engine pulls its DMA handle from gUdmaDrvObjPtr. Done ONCE for the lifetime. */
    rcSci = appSciclientInit(APP_IPC_CPU_C7x_1);
    OW_DBG[1] = (uint32_t)rcSci;
    appUdmaInitPrmSetDefault(&udmaPrm);
    rcUdma = appUdmaInit(&udmaPrm);
    OW_DBG[2] = (uint32_t)rcUdma;
    gUdmaDrvObjPtr = appUdmaGetObj();
    OW_DBG[3] = (uint32_t)(uintptr_t)gUdmaDrvObjPtr;

    /* 2. Multi-invoke service loop. Per request the A72 stages input (+ net/io, unchanged) and
     *    sets READY; we consume READY, run inference, publish output + set DONE. The network is
     *    created ONCE on the first kick and reused (load-once / run-many). A different net needs
     *    a reboot (or a future delete+recreate when NETLEN changes). */
    for (;;)
    {
        OW_DBG[4] = 0xABCD0002u;                    /* waiting for READY */
        while (RD32(OW_MB_READY) != OW_MB_READY_MAGIC) { /* spin */ }

        /* 3. Create the network instance on the first kick (net/io are staged by then). */
        if (handle == NULL)
        {
            netLen = RD32(OW_MB_NETLEN);
            ioLen  = RD32(OW_MB_IOLEN);
            OW_DBG[5] = netLen;
            OW_DBG[6] = 0xABCD0003u;                /* pre-create */
            TIDLRT_setParamsDefault(&prms);
            prms.netPtr       = (void *)(uintptr_t)OW_MB_NET;
            prms.ioBufDescPtr = (void *)(uintptr_t)OW_MB_IO;
            prms.net_capacity = (int32_t)netLen;
            prms.io_capacity  = (int32_t)ioLen;
            prms.flowCtrl     = 0;
            prms.traceLogLevel= 0;
            prms.stats        = NULL;
            status = TIDLRT_create(&prms, &handle);
            OW_DBG[7] = (uint32_t)status;           /* create status */
            if (status != 0)
            {
                WR32(OW_MB_STATUS, (uint32_t)status);
                WR32(OW_MB_READY, 0);
                WR32(OW_MB_DONE, OW_MB_DONE_MAGIC); /* report the failure, then stop serving */
                return status;
            }

            /* 4. Bind float tensors once. elementType=Float32 (==TIDL_SinglePrecFloat=6)
             *    deliberately does NOT match the net's quantized inElementType, so tidl_rt.c takes
             *    the layout-aware conversion path (float in -> quantized buf; quantized out ->
             *    float). Dims come from the io descriptor, so only ptr/elementType/layout matter. */
            TIDLRT_setTensorDefault(&inT);
            TIDLRT_setTensorDefault(&outT);
            inT.ptr         = s_in;
            inT.elementType = TIDLRT_Float32;
            inT.layout      = TIDLRT_LT_NCHW;
            outT.ptr        = s_out;
            outT.elementType= TIDLRT_Float32;
            outT.layout     = TIDLRT_LT_NCHW;
        }

        /* 5. Pull this kick's input, run inference (engine + MMA). */
        memcpy(s_in, (void *)(uintptr_t)OW_MB_INPUT, sizeof(s_in));
        OW_DBG[8] = 0xABCD0004u;                    /* pre-invoke */
        status = TIDLRT_invoke(handle, in, out);
        OW_DBG[9] = (uint32_t)status;               /* invoke status */
        WR32(OW_MB_STATUS, (uint32_t)status);

        /* 6. Publish float output, bump the invoke counter, consume READY, then signal DONE
         *    (clear READY BEFORE DONE so the next host kick is always a clean 0->magic edge). */
        memcpy((void *)(uintptr_t)OW_MB_OUTPUT, s_out, sizeof(s_out));
        WR32(OW_MB_INVOKES, ++nInvoke);
        WR32(OW_MB_READY, 0);
        WR32(OW_MB_DONE,  OW_MB_DONE_MAGIC);
    }
    /* not reached */
}
