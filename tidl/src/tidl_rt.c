/* openWearable: VENDORED from PSDK-RTOS 10.01.00.04 c7x-mma-tidl/ti_dl/test/src/tidl_rt.c.
 * OW change: removed the test harness's pFxnUnLock 0xDA scratch-corruption + restored real
 * HwiP_disable/restore (the bug that stalled the DRU). See tidl/src/PROVENANCE.md. TI header below. */
/*
*
* Copyright (c) {2015 - 2020} Texas Instruments Incorporated
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



/** @file tidl_tb.c
*
*  @brief  This file contains test code for TI DL RT API in Tets application
*
*
*  @date   Jul 2020
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#if (HOST_EMULATION)
#include <malloc.h>
#endif

#include <ivision.h>
#include <HwiP.h>   /* openWearable: real HwiP_disable/restore for pFxnLock/UnLock */
#include "itidl_ti.h"
#include "itidl_rt.h"
#include "platform_common.h"
#include "ti_mem_manager.h"
#include "tidl_custom.h"
#include "tidl_tb_utils.h"
extern void Cache_wbInvAll(void); /*OW cache flush decl*/

#if defined(MCU_PLUS_SDK)
#include <drivers/hw_include/cslr_soc.h>
#else
#include <ti/csl/soc.h>
#endif

int32_t gComparisonStatus = 0;
#ifndef HOST_EMULATION
//#define TIDL_UC_AVV_BUILD
#endif

uint32_t coreId = 0;  //Valid only for J784S4, used only in AVV BUILD, set to corresponding correct core Id in main function of dsp_nonos build
//UC VARIABLES:
#ifdef TIDL_UC_AVV_BUILD

#define MULTICORE_SYNC_BASE      (0x41C28000)
#define C7_0_WR_OFFSET           (0x0 + (coreId)) // word-addressing
#define C7_0_RD_OFFSET           (0x20 + (coreId)) // word-addressing

#define SMALL_WAIT_TIME          (0x64)
#define MULTICORE_SYNC_VAL       (0x12345678)

#define TIDL_DBUG_CTRL_BASE      (0x82F0F000 + 0x3000000*(coreId)) 

#define MULTICORE_CSI_COUNTER    (0x41C28150)
#define STATUS_REPORT            (0x41C28100)
#endif

void tidl_printStatus(int32_t status);
extern int gWriteStaticBins;
extern int currentNetwork;

/* Ideally shall use the TIDLVprintf */
int tidl_tb_printf(int traceLevel, const char *format, ...);
int32_t tidltb_getDatElementSize(int32_t elementType);
const sTIDL_DataParams_t *tidl_getDataBufDims(const sTIDL_Network_t *net, int32_t dataBuffId);

void* aligned_malloc(size_t required_bytes, size_t alignment)
{
    void* p1; // original block
    void** p2; // aligned block
    int offset = alignment - 1 + sizeof(void*);
    if ((p1 = (void*)malloc(required_bytes + offset)) == NULL)
    {
       return NULL;
    }
    p2 = (void**)(((size_t)(p1) + offset) & ~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void aligned_free(void *p)
{
    free(((void**)p)[-1]);
}

#define TIDLTB_ASSERT_MSG(x)   {\
    if(!(x)) \
    {\
      tidl_tb_printf(0, "Error at line: %5d : in file %22s, of function : %s \n", __LINE__, __FILE__,__FUNCTION__);\
    }\
  }

#define TIDLTB_ASSERT_EXIT(x)  {\
        if(!(x)) \
        {\
          tidl_tb_printf(0, "Error at line: %5d : in file %22s, of function : %s \n", __LINE__, __FILE__,__FUNCTION__);\
          exit(-1);\
        }\
      }

#if (HOST_EMULATION)
  #if defined (__C7100__) || defined (__C7120__) || defined (__C7504__)  || defined (__C7524__)
    #include <c7x.h>
  #endif /* __C7100__ */
  #define _TSC_read() //
  #include <time.h>
#else
  #if defined (__C7100__) || defined (__C7120__) || defined (__C7504__)   || defined (__C7524__)
    #include <c7x.h>
  #endif /* __C7100__ */
  #define _TSC_read() __TSC;
  #ifdef DDR_BW_STATS
    #define ENABLE_DDR_BW_STATS          (1)
  #else
    #define ENABLE_DDR_BW_STATS          (0)
  #endif
#endif

uint64_t ddr_read, ddr_write;
uint64_t gFinalCycles = 0;
typedef struct IM_Fxns
{
  IVISION_Fxns * ivision;

} IM_Fxns;

/**
@struct  sTIDLRTTB_IntHandle_t
@brief   This structure is internal handle for TIDL RT API wrapper
*/
typedef struct
{
  /** Pointer to Ivision memory record Array  */
  IALG_MemRec *memRec;
  /** Pointer to Ivision algorithm instance handle */
  IM_Fxns     *algHandle;
  /** TIRL RT Craete time configuration parametes*/
  sTIDLRT_Params_t rtPrms;
  /** TIDL algorithm create time Parameters */
  TIDL_CreateParams  createParams;
  /** NCurrent processing /Invoke Index  */
  int32_t procIdx;
} sTIDLRTTB_IntHandle_t;

/* read EMIF counter and calculate read and write bytes since last read */
#define APP_PERF_DDR_BURST_SIZE_BYTES   (  64u)  /* in units of bytes */
void tidl_ddrStatsReadCounters(uint64_t *read_bytes, uint64_t *write_bytes)
{  
  *write_bytes = 0;
  *read_bytes = 0;
#if (!HOST_EMULATION)  
#if ENABLE_DDR_BW_STATS  
    static uint32_t is_first_time = 1;
    static volatile uint32_t *cnt_sel = (volatile uint32_t *)0x02980100;
    static volatile uint32_t *cnt0    = (volatile uint32_t *)0x02980104;
    static volatile uint32_t *cnt1    = (volatile uint32_t *)0x02980108;
    static volatile uint32_t last_cnt0 = 0, last_cnt1 = 0;
    volatile uint32_t cur_cnt0, cur_cnt1;
    uint32_t diff_cnt0, diff_cnt1;

    if(is_first_time)
    {
        is_first_time = 0;

        /* cnt0 is counting reads, cnt1 is counting writes, cnt2, cnt3 not used */
        *cnt_sel = (0u<<0u) | (1u<<8u) | (2u<<16u) | (3u<<24u);
        last_cnt0 = *cnt0;
        last_cnt1 = *cnt1;
    }

    cur_cnt0 = *cnt0;
    cur_cnt1 = *cnt1;
    if(cur_cnt0 < last_cnt0)
    {
        /* wrap around case */
        diff_cnt0 = (0xFFFFFFFFu - last_cnt0) + cur_cnt0;
    }
    else
    {
        diff_cnt0 = cur_cnt0 - last_cnt0;
    }
    if(cur_cnt1 < last_cnt1)
    {
        /* wrap around case */
        diff_cnt1 = (0xFFFFFFFFu - last_cnt1) + cur_cnt1;
    }
    else
    {
        diff_cnt1 = cur_cnt1 - last_cnt1;
    }
    last_cnt0 = cur_cnt0;
    last_cnt1 = cur_cnt1;

    *write_bytes = (uint64_t)diff_cnt0 * APP_PERF_DDR_BURST_SIZE_BYTES;
    *read_bytes = (uint64_t)diff_cnt1 * APP_PERF_DDR_BURST_SIZE_BYTES;
#endif
#endif
  return;
}

void tidl_printDdrStats(uint64_t read_bytes, uint64_t write_bytes)
{
  
#if (!HOST_EMULATION)
  #if ENABLE_DDR_BW_STATS
      tidl_tb_printf(0, " DDR_READ_WRITE_TOTAL_MB = %8.2f, %8.2f, %8.2f ", (ddr_read / 1000000.0), (ddr_write / 1000000.0), ((ddr_read + ddr_write) / 1000000.0));
  #endif
#endif
} 

#if (!HOST_EMULATION)
uint64_t tsc_start, tsc_end;
#else
clock_t clock_start, clock_end;
#endif

static void tidl_profileMarkerStart()
{
  #if (!HOST_EMULATION)
      tsc_start = _TSC_read();
  #else
      clock_start = clock();
  #endif
}


static void tidl_profileMarkerEnd()
{
  #if (HOST_EMULATION)
      clock_end = clock();
  #else
      tsc_end = _TSC_read();
  #endif
}

static void tidl_printProfileInfo()
{
#if (HOST_EMULATION)      
    double elapsed_time = (clock_end - clock_start) / (double)CLOCKS_PER_SEC;
    tidl_tb_printf(0, " T %10.2f ", elapsed_time * 1000);
#else
    int64_t tsc_cycles;
    if (tsc_end < tsc_start)
    {      
      tsc_cycles = (ULONG_MAX  + tsc_end - tsc_start);
      tidl_tb_printf(0, " TSC Mega Cycles = %8.2f with Over Flow ", (tsc_cycles / 1000000.0));
    }
    else
    {
      tsc_cycles = tsc_end - tsc_start;             
      tidl_tb_printf(0, " TSC Mega Cycles = %8.2f ", (tsc_cycles / 1000000.0));        
    }
    gFinalCycles = tsc_cycles;
#endif
}


TIMemObject memObj_DMEM0;
TIMemObject memObj_DMEM1;
TIMemObject memObj_SARAM0;
TIMemObject memObj_EXTMEM;
extern void* tidltb_alignMalloc(int32_t size, int32_t alignment);

extern void tidltb_alignFree(void * ptr);


int32_t TestApp_AllocMemRecords(sTIDLRT_Params_t *prms, IALG_MemRec * memRec,int32_t numMemRec)
{
  int32_t i;
  int32_t totalDdrSize = 0;
  (*(volatile unsigned int*)0xb80000b0u)=(unsigned int)numMemRec; /*OW memrec probe*/
  TIMemHandle memHdl_DMEM0 = &memObj_DMEM0;
  TIMemHandle memHdl_DMEM1 = &memObj_DMEM1;
  TIMemHandle memHdl_SARAM0 = &memObj_SARAM0;
  TIMemHandle memHdl_EXTMEM = &memObj_EXTMEM;



  for (i = 0; i < numMemRec; i++)
  {
    if(memRec[i].space == IALG_DARAM0) /* L1 D*/
    {
      memRec[i].base = TI_GetMemoryChunk(memHdl_DMEM0, memRec[i].size,
        memRec[i].alignment);
    }
    else if(memRec[i].space == IALG_DARAM1) /* L2 SRAM*/
    {
      memRec[i].base = TI_GetMemoryChunk(memHdl_DMEM1, memRec[i].size,
        memRec[i].alignment);
    }
    else if(memRec[i].space == IALG_SARAM0) /* L3 MSMC SRAM*/
    {
      memRec[i].base = TI_GetMemoryChunk(memHdl_SARAM0, memRec[i].size,
        memRec[i].alignment);
    }
    else if(((memRec[i].space == IALG_EXTERNAL_NON_CACHEABLE) || (memRec[i].space == IALG_EXTERNAL)) && (memRec[i].attrs == IALG_SCRATCH) &&
            ((prms->flowCtrl & 2) == 0))/* SCRATCH EXTERNAL MEM*/
    {
      memRec[i].base = TI_GetMemoryChunk(memHdl_EXTMEM, memRec[i].size,
        memRec[i].alignment);
    }
    else
    {
      memRec[i].base = (void *) tidltb_alignMalloc( memRec[i].size, memRec[i].alignment);

    }
    if(memRec[i].base == NULL)
    {
     (*(volatile unsigned int*)0xb80000a0u)=(unsigned int)i;            /*OW memrec probe*/
     (*(volatile unsigned int*)0xb80000a4u)=(unsigned int)memRec[i].size;
     (*(volatile unsigned int*)0xb80000a8u)=(unsigned int)memRec[i].space;
     (*(volatile unsigned int*)0xb80000acu)=(unsigned int)memRec[i].attrs;
     tidl_tb_printf(0, "Could not Allocate memory for memtab %d of size %d in %d\n", i, memRec[i].size,memRec[i].space);
     return IALG_EFAIL;
    }
  }

  (*(volatile unsigned int*)0xb80000b4u)=0x0A110A11u; /*OW memrec probe: loop ok*/
  tidl_tb_printf(2, "Num,    Space,     SizeinBytes,   SineInMB\n");
  for (i = 0; i < numMemRec; i++)
  {
    tidl_tb_printf(2, " %3d, %5d, %12d,    %7.3f %p\n", i, memRec[i].space, memRec[i].size, memRec[i].size / (1024.0 * 1024), memRec[i].base);
    if((memRec[i].space == IALG_EXTERNAL_NON_CACHEABLE) || (memRec[i].space == IALG_EXTERNAL))
      totalDdrSize += memRec[i].size;
  }
  tidl_tb_printf(2, "Total External Memory (DDR) Size = %12d,    %7.3f \n", totalDdrSize, totalDdrSize / (1024.0 * 1024));
  
  return IALG_EOK;
}

int32_t TestApp_FreeMemRecords(sTIDLRT_Params_t *prms, IALG_MemRec * memRec,int32_t numMemRec)
{
  int32_t i;
  TIMemHandle memHdl_DMEM0 = &memObj_DMEM0;
  TIMemHandle memHdl_DMEM1 = &memObj_DMEM1;
  TIMemHandle memHdl_SARAM0 = &memObj_SARAM0;
  TIMemHandle memHdl_EXTMEM = &memObj_EXTMEM;

  for (i = 0; i < numMemRec; i++)
  {
    if(memRec[i].base == NULL)
    {
      return IALG_EFAIL;
    }
    if(memRec[i].space == IALG_DARAM0) {
      TI_ResetMemoryHandle(memHdl_DMEM0);
    }
    else if(memRec[i].space == IALG_DARAM1) {
      TI_ResetMemoryHandle(memHdl_DMEM1);
    }
    else if(memRec[i].space == IALG_SARAM0) {
      TI_ResetMemoryHandle(memHdl_SARAM0);
    }
    else if(((memRec[i].space == IALG_EXTERNAL_NON_CACHEABLE) || (memRec[i].space == IALG_EXTERNAL)) && (memRec[i].attrs == IALG_SCRATCH) &&
            ((prms->flowCtrl & 2) == 0)) {
      TI_ResetMemoryHandle(memHdl_EXTMEM);
    }
    else {
      tidltb_alignFree(memRec[i].base);
    }
  }
  return IALG_EOK;
}

int32_t TIDLRT_setTensorDefault(sTIDLRT_Tensor_t *tensor)
{
    int32_t status                  = 0;
    //tensor->name[]                = {0};
    tensor->elementType             = 0;
    tensor->numDim                  = 0;
    tensor->ptr                     = NULL;
    tensor->dataOffset              = 0;
    tensor->layout                  = 0;
    tensor->zeroPoint               = 0;
    tensor->scale                   = 1.0;
    tensor->memType                 = 0;
    tensor->pitch[TIDL_ROI_PITCH]     = -1;
    tensor->pitch[TIDL_CHANNEL_PITCH] = -1;
    tensor->pitch[TIDL_DIM1_PITCH]    = -1;
    tensor->pitch[TIDL_DIM2_PITCH]    = -1;
    tensor->padValues[0] = 0;
    tensor->padValues[1] = 0;
    tensor->padValues[2] = 0;
    tensor->padValues[3] = 0;
    tensor->dimValues[TIDL_DIM_WIDTH]   = -1;
    tensor->dimValues[TIDL_DIM_HEIGHT]  = -1;
    tensor->dimValues[TIDL_DIM_NUMCH]   = -1;
    tensor->dimValues[TIDL_DIM_BATCH]   = -1;
    tensor->bufferSize = -1;

    tidl_tb_printf(2, "TIDL_RT: Set default TIDLRT tensor done\n");
    return status;

}

void tidl_FreeNetInputMem(sTIDL_IOBufDesc_t * ioPrms, IVISION_BufDesc *BufDescList)
{
#if !BUILD_WITH_STATIC_ARRAYS 
  uint16_t numBuffs = 0;
  for(numBuffs = 0; numBuffs < ioPrms->numInputBuf; numBuffs++)
  {
    if(BufDescList[numBuffs].bufPlanes[0].buf)
    {
#if defined SOC_J784S4 || defined(SOC_J742S2)
      aligned_free(BufDescList[numBuffs].bufPlanes[0].buf);
#else
      free(BufDescList[numBuffs].bufPlanes[0].buf);
#endif
    }
  }
#endif
}

void tidl_FreeNetOutputMem(sTIDL_IOBufDesc_t * ioPrms, IVISION_BufDesc *BufDescList)
{
  uint16_t numBuffs = 0;
  for(numBuffs = 0; numBuffs < ioPrms->numOutputBuf; numBuffs++)
  {
    if(BufDescList[numBuffs].bufPlanes[0].buf)
    {
#if defined SOC_J784S4 || defined(SOC_J742S2)
      aligned_free(BufDescList[numBuffs].bufPlanes[0].buf); 
#else
      free(BufDescList[numBuffs].bufPlanes[0].buf);
#endif
    }
  }
}

int32_t tidl_AllocNetInputMem(sTIDL_IOBufDesc_t * ioPrms, IVISION_BufDesc *BufDescList)
{
  uint16_t numBuffs = 0;
  int32_t bufWidth;
  int32_t bufHeight;

  for(numBuffs = 0; numBuffs < ioPrms->numInputBuf; numBuffs++)
  {
    int32_t elementSizeBytes  = tidltb_getDatElementSize(ioPrms->inElementType[numBuffs]);
    BufDescList[numBuffs].numPlanes                          = 1;
    BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.x    = 0;
    BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.y    = 0;


    bufWidth  = ioPrms->inWidth[numBuffs] + ioPrms->inPadL[numBuffs];
    bufHeight = ioPrms->inNumBatches[numBuffs]  * ioPrms->inNumChannels[numBuffs] * ioPrms->inDIM1[numBuffs] * ioPrms->inDIM2[numBuffs] *
      (ioPrms->inHeight[numBuffs] + ioPrms->inPadT[numBuffs] + ioPrms->inPadB[numBuffs]);


    BufDescList[numBuffs].bufPlanes[0].width  = bufWidth;
    BufDescList[numBuffs].bufPlanes[0].height = bufHeight;
    BufDescList[numBuffs].bufPlanes[0].frameROI.width        =
              ioPrms->inWidth[numBuffs];
    BufDescList[numBuffs].bufPlanes[0].frameROI.height       =
              ioPrms->inHeight[numBuffs];

#if (!BUILD_WITH_STATIC_ARRAYS)
    uint32_t inBufSize = ioPrms->inNumBatches[numBuffs]  *ioPrms->inDIM1[numBuffs]  *ioPrms->inDIM2[numBuffs]  *  ioPrms->inChannelPitch[numBuffs] * (ioPrms->inNumChannels[numBuffs] + 1) * elementSizeBytes;

#if HOST_EMULATION
    /* Allocation extra in Host emualtion mode to avoid read access voilation */
    inBufSize *= 2;
#endif

#if defined SOC_J784S4 || defined(SOC_J742S2)
    BufDescList[numBuffs].bufPlanes[0].buf = (int8_t *)aligned_malloc(inBufSize, 128);
#else 
    BufDescList[numBuffs].bufPlanes[0].buf = (int8_t *)malloc(inBufSize);
#endif
    tidl_tb_printf(2, "%12d,    %7.3f %p\n", inBufSize, inBufSize / (1024.0 * 1024), BufDescList[numBuffs].bufPlanes[0].buf);
    TIDLTB_ASSERT_EXIT(BufDescList[numBuffs].bufPlanes[0].buf != NULL);
    memset(BufDescList[numBuffs].bufPlanes[0].buf,0,inBufSize);

#endif
    BufDescList[numBuffs].reserved[0] =
      ioPrms->inDataId[numBuffs];
    BufDescList[numBuffs].bufferId = ioPrms->inDataId[numBuffs];
  }
  return numBuffs;
}

int32_t tidl_AllocNetOutputMem(sTIDL_IOBufDesc_t * ioPrms, IVISION_BufDesc   *BufDescList)
{

  uint16_t numBuffs = 0;
  uint32_t outputMemRequired;

  for(numBuffs = 0; numBuffs < ioPrms->numOutputBuf; numBuffs++)
  {
      int32_t elementSizeBytes  = tidltb_getDatElementSize(ioPrms->outElementType[numBuffs]);
      BufDescList[numBuffs].numPlanes                          = 1;
      BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.x    = 0;
      BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.y    = 0;

      int32_t imHeight      = ioPrms->outHeight[numBuffs];
      int32_t imWidth       = ioPrms->outWidth[numBuffs];

      BufDescList[numBuffs].bufPlanes[0].width                 =
               imWidth + ioPrms->outPadL[numBuffs];
      BufDescList[numBuffs].bufPlanes[0].height                =
               ioPrms->outNumBatches[numBuffs] * ioPrms->outNumChannels[numBuffs]* ioPrms->outDIM1[numBuffs] * ioPrms->outDIM2[numBuffs] *
              (imHeight + ioPrms->outPadT[numBuffs] + ioPrms->outPadB[numBuffs]);
      BufDescList[numBuffs].bufPlanes[0].frameROI.width        = imWidth;
      BufDescList[numBuffs].bufPlanes[0].frameROI.height       = imHeight;

      outputMemRequired = ioPrms->outNumBatches[numBuffs] *ioPrms->outDIM1[numBuffs] *ioPrms->outDIM2[numBuffs] * (ioPrms->outNumChannels[numBuffs] + ioPrms->outPadCh[numBuffs]+1)* ioPrms->outChannelPitch[numBuffs]*elementSizeBytes;
#if defined SOC_J784S4 || defined(SOC_J742S2)
      BufDescList[numBuffs].bufPlanes[0].buf = (int8_t *)aligned_malloc(outputMemRequired*elementSizeBytes, 128);
#else
      BufDescList[numBuffs].bufPlanes[0].buf = (int8_t *)malloc(outputMemRequired*elementSizeBytes);
#endif
      TIDLTB_ASSERT_EXIT(BufDescList[numBuffs].bufPlanes[0].buf != NULL)
      tidl_tb_printf(2, "%12d,    %7.3f %p\n", outputMemRequired*elementSizeBytes, (outputMemRequired*elementSizeBytes) / (1024.0 * 1024), BufDescList[numBuffs].bufPlanes[0].buf);


      BufDescList[numBuffs].reserved[0]      = ioPrms->outDataId[numBuffs];

      if((BufDescList[numBuffs].bufPlanes[0].width *
          BufDescList[numBuffs].bufPlanes[0].height*elementSizeBytes) <  (16*1024))
      {
        memset( BufDescList[numBuffs].bufPlanes[0].buf,0,
          BufDescList[numBuffs].bufPlanes[0].width *
          BufDescList[numBuffs].bufPlanes[0].height*elementSizeBytes);
      }
      BufDescList[numBuffs].bufferId         = ioPrms->outDataId[numBuffs];
  }
  return numBuffs;
}

int32_t is_tidlrt_in_tensor_same( sTIDL_IOBufDesc_t *ioBufDesc, sTIDLRT_Tensor_t *in[], uint32_t id, uint32_t tidlrt_id)
{
    if(in[tidlrt_id]->elementType != ioBufDesc->inElementType[id])
    {
        return 0;
    }
    int32_t chPitch = in[tidlrt_id]->pitch[TIDL_CHANNEL_PITCH];
    if(chPitch == -1)
    {
        chPitch = ioBufDesc->inWidth[id]*ioBufDesc->inHeight[id];
    }
    if(chPitch != ioBufDesc->inChannelPitch[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->layout != ioBufDesc->inLayout[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->zeroPoint != ioBufDesc->inZeroPoint[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->scale != ioBufDesc->inTensorScale[id])
    {
        return 0;
    }
    return 1;
}

int32_t is_tidlrt_out_tensor_same( sTIDL_IOBufDesc_t *ioBufDesc, sTIDLRT_Tensor_t *in[], uint32_t id, uint32_t tidlrt_id)
{
    if(in[tidlrt_id]->elementType != ioBufDesc->outElementType[id])
    {
        return 0;
    }
    int32_t chPitch = in[tidlrt_id]->pitch[TIDL_CHANNEL_PITCH];
    if(chPitch == -1)
    {
        chPitch = ioBufDesc->outWidth[id]*ioBufDesc->outHeight[id];
    }
    if(chPitch != ioBufDesc->outChannelPitch[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->layout != ioBufDesc->outLayout[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->zeroPoint != ioBufDesc->outZeroPoint[id])
    {
        return 0;
    }
    if(in[tidlrt_id]->scale != ioBufDesc->outTensorScale[id])
    {
        return 0;
    }
    return 1;
}

static void tidl_copyRTTensorToInBufs(const sTIDL_Network_t *net, sTIDL_IOBufDesc_t * ioPrms,  sTIDLRT_Tensor_t   *ins[], IVISION_BufDesc   *BufDescList)
{
  #if (!BUILD_WITH_STATIC_ARRAYS)
  int i;
  for (i = 0; i < ioPrms->numInputBuf; i++)
  {
    int32_t c = ioPrms->inNumChannels[i];
    int32_t d2 = ioPrms->inDIM2[i];
    int32_t d1 = ioPrms->inDIM1[i];
    int32_t n = ioPrms->inNumBatches[i];
    if(is_tidlrt_in_tensor_same(ioPrms, ins, i, i))
    {
      void *rtPtr  = ins[i]->ptr;
      void * ivPtr = BufDescList[i].bufPlanes[0].buf;
      int32_t elementSizeBytes  = tidltb_getDatElementSize(ins[i]->elementType);
      memcpy(ivPtr, rtPtr, n * d1 * d2 * c * elementSizeBytes * ins[i]->pitch[TIDL_CHANNEL_PITCH]);
    }
    else
    {
      /** Needs to be updated for 6D*/
      int32_t h, w, lp, cp;
      int32_t i0, i1, i2, i3, offset, idx;
      float data;
      w = ioPrms->inWidth[i];
      h = ioPrms->inHeight[i];
      lp = BufDescList[i].bufPlanes[0].width  ;
      cp = lp* (BufDescList[i].bufPlanes[0].height/(c*n));
      void *rtPtr  = ins[i]->ptr;
      void * ivPtr = BufDescList[i].bufPlanes[0].buf;
      offset = lp*ioPrms->inPadT[i] + ioPrms->inPadL[i];
      const sTIDL_DataParams_t * dataPrms = tidl_getDataBufDims(net, ioPrms->inDataId[i]);
      float scale = dataPrms->tensorScale;
      float inScale = ins[i]->scale;
      int32_t zp = ins[i]->zeroPoint;
      int32_t layout = ins[i]->layout;

      if(ins[i]->elementType == TIDL_SinglePrecFloat)
      {
        inScale = 1.0;
        zp = 0;
      }
      for(i3 = 0; i3 < n; i3++)
      {
      for(i0 = 0; i0 < c; i0++)
      {
        for(i1 = 0; i1 < h; i1++)
        {
          for(i2 = 0; i2 < w; i2++)
          {
            if(layout == TIDLRT_LT_NCHW)
            {
              idx =  i3*h*w*c + i0*h*w + i1*w + i2;
            }
            else
            {
              idx =  i3*h*w*c + i0 + i1*w*c + i2*c;
            }
            if(ins[i]->elementType ==  TIDLRT_Uint8)
            {
              data =   ((uint8_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType ==  TIDLRT_Int8)
            {
              data = ((int8_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType ==  TIDLRT_Uint16)
            {
              data = ((uint16_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType ==  TIDLRT_Int16)
            {
              data = ((int16_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType == TIDLRT_Float32)
            {
              data = ((float*)rtPtr)[idx];
            }
            else if(ins[i]->elementType == TIDLRT_Uint32)
            {
              data = ((uint32_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType == TIDLRT_Int32)
            {
              data = ((int32_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType == TIDLRT_Uint64)
            {
              data = ((uint64_t*)rtPtr)[idx];
            }
            else if(ins[i]->elementType == TIDLRT_Int64)
            {
              data = ((int64_t*)rtPtr)[idx];
            }
            else
            {
              TIDLTB_ASSERT_MSG(0);
            }
            if((inScale != scale) || (zp != 0))
            {
              data = ((data - zp)/inScale) * scale;
            }

            if(ioPrms->inElementType[i] ==  TIDL_UnsignedChar)
            {
              ((uint8_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_SignedChar)
            {
              ((int8_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_UnsignedShort)
            {
              ((uint16_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_SignedShort)
            {
              ((int16_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] == TIDL_SinglePrecFloat)
            {
              ((float*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_UnsignedWord)
            {
              ((uint32_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_SignedWord)
            {
              ((int32_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_UnsignedDoubleWord)
            {
              ((uint64_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else if(ioPrms->inElementType[i] ==  TIDL_SignedDoubleWord)
            {
              ((int64_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2] = data;
            }
            else
            {
              TIDLTB_ASSERT_MSG(0);
            }
          }
        }
      }
      }
    }

  }
#endif
}

static int32_t tidl_genCompStaticOutput(sTIDL_IOBufDesc_t * ioPrms, IVISION_BufDesc   *BufDescList)
{
  int i = 0;
  int32_t isMatching  = 1;
  int32_t elementSizeBytes;
  uint32_t outBufSize;
  int32_t tensorSize;
  int32_t tensorOffset;
  uint32_t totalSize = 0;
  for(i = 0; i < ioPrms->numOutputBuf; i++)
  {
    elementSizeBytes  = tidltb_getDatElementSize(ioPrms->outElementType[i]);
    outBufSize = ioPrms->outChannelPitch[i] * ioPrms->outNumChannels[i] * ioPrms->outDIM2[i] * ioPrms->outDIM1[i] * elementSizeBytes;
   // printf("IV Buffer %d : C = %d CPITCH + %d\n",ioPrms->inChannelPitch[i],ioPrms->inNumChannels[i]);
    totalSize += outBufSize;
  }
#if (!BUILD_WITH_STATIC_ARRAYS)
  //Generate for Non-Static Builds
  if (gWriteStaticBins)
  {
    tidl_tb_printf(0, "Writing Static Headers for Output Tensors\n");
    uint8_t * outPtr = NULL;
    outPtr = (uint8_t *)malloc(totalSize);
    TIDLTB_ASSERT_EXIT(outPtr != NULL);
    tensorOffset = 0;
    for (i = 0; i < ioPrms->numInputBuf; i++)
    {
      void * ivPtr = BufDescList[i].bufPlanes[0].buf;//Ivision Pointer is set at this point
      elementSizeBytes  = tidltb_getDatElementSize(ioPrms->inElementType[i]);
      tensorSize = ioPrms->outChannelPitch[i] * ioPrms->outNumChannels[i] * elementSizeBytes;
      memcpy(outPtr + tensorOffset, ivPtr, tensorSize);
      tensorOffset += tensorSize;
    }
    //Depricate old method.
    //tidl_writeAsStaticArray((uint8_t *)outPtr, (int8_t *)"./tidl_tb_pOutx_tensor", tensorOffset, "tidl_tb_pOutx_tensor", "tidl_tb_pOutx_tensor", 1U, 0);
    tidl_writeMultiNetworkBin((uint8_t*)outPtr, (int8_t *)"./tidl_tb_pOutx_tensor", tensorOffset);
    if ( outPtr )
    {
      free(outPtr);
    }
  }
  else
  {
    tidl_tb_printf(2,"Skipping static gen-set function\n");
  }
#else
  //OUTPUT COMPARISON STARTS HERE!
  //In the static build, we have "ivPtr for output + the static data"
  tidl_buffer_printf_dump();
  tidl_tb_printf(0, "Starting out comparison Output Tensors : %d\n",ioPrms->numOutputBuf);
  tensorOffset = 0;
  int32_t status = 10;
  for (i = 0; i < ioPrms->numOutputBuf; i++)
  {
    elementSizeBytes  = tidltb_getDatElementSize(ioPrms->inElementType[i]);
    tensorSize = ioPrms->outChannelPitch[i] * ioPrms->outNumChannels[i] * elementSizeBytes;
    void * ivPtr = BufDescList[i].bufPlanes[0].buf; //OUTPTR GENERATED BY NW
    int32_t localOffset = tidl_getStaticOffset(tidl_tb_pOutx_tensor); //To jump to a particular n/w's data.
    tidl_tb_printf(0, "Comparison  b/w %p (REF) and %p (GEN) for %d bytes\n",(tidl_tb_pOutx_tensor + tensorOffset + localOffset),ivPtr,tensorSize);
    status = memcmp((void*)(tidl_tb_pOutx_tensor + tensorOffset + localOffset),ivPtr, tensorSize);
    if (status != 0)
    {
      isMatching = 0;
      gComparisonStatus = -1;
      tidl_tb_printf( 0 , "Failure at position %d for tensor %d\n",status, i);
      break;
    }

    //BufDescList[i].bufPlanes[0].buf = (void *)(tidl_tb_pOutx_tensor + tensorOffset);
    tensorOffset += tensorSize;
    tidl_tb_printf(0,"TensorSize IN %d = %d\n",i,tensorSize);
  }
  if(status == 0)
  tidl_tb_printf(1, "TIDL_DBUG_TRACE :  %s, %s, %d PASSED n", __FUNCTION__, __FILE__, __LINE__);
    gComparisonStatus = 1;
    isMatching = 1;
    tidl_tb_printf(0, "SUCCESSFUL COMPARISON!\n");
  tidl_tb_printf(0, "Comparison done with status %d!\n",status);
  
#endif
  return isMatching;
}


/*
int32_t tidl_CompareNetOutputMem(tidl_net_config *params, sTIDL_IOBufDesc_t * ioPrms, sTIDLRT_Tensor_t * out[])
{
  int32_t i;
//  uint16_t nuChs;
 // uint32_t pitch;
  //uint32_t chOffset;
  int8_t * readPtr;
  int32_t status, tensorOffset = 0, readSize = 0;

  for(i = 0; i < ioPrms->numOutputBuf; i++)
  {
    readSize  =  ioPrms->outNumChannels[i] * ioPrms->outHeight[i] * ioPrms->outWidth[i];
    tidl_tb_printf(0,"OUTCMP_DIM= %d %d %d [CHW]\n",ioPrms->outNumChannels[i],ioPrms->outHeight[i],ioPrms->outWidth[i]);
    readSize *= tidltb_getDatElementSize(ioPrms->outElementType[i]);;
    readPtr = (int8_t *)tidl_tb_output_tensor + tensorOffset;
    tensorOffset += readSize;


    if (status)
    {
      tidl_tb_printf(0, "Failure at position %d for tensor %d\n",status, i);
    }

  }

  if(status == 0)
  tidl_tb_printf(0, "TIDL_DBUG_TRACE :  %s, %s, %d PASSED n", __FUNCTION__, __FILE__, __LINE__);

  return 0;*/

void tidl_copyOutBufsToRTTensor(sTIDL_IOBufDesc_t * ioPrms,  sTIDLRT_Tensor_t   *ins[], IVISION_BufDesc   *BufDescList, float * scales)
{
  int i;
  for (i = 0; i < ioPrms->numOutputBuf; i++)
  {
    int32_t c = ioPrms->outNumChannels[i];
    int32_t d2 = ioPrms->outDIM2[i];
    int32_t d1 = ioPrms->outDIM1[i];
    int32_t n = ioPrms->outNumBatches[i];
    if(is_tidlrt_out_tensor_same(ioPrms, ins, i, i))
    {
      void *rtPtr  = ins[i]->ptr;
      void * ivPtr = BufDescList[i].bufPlanes[0].buf;
      int32_t elementSizeBytes  = tidltb_getDatElementSize(ins[i]->elementType);
      memcpy(rtPtr, ivPtr, n * d1 * d2 * c * elementSizeBytes * ins[i]->pitch[TIDL_CHANNEL_PITCH]);
   }
    else
    {
      int32_t h, w, lp, cp;
      int32_t i0, i1, i2, i3, offset, zp, idx;
      float data, outScale;
      w = ioPrms->outWidth[i];
      h = ioPrms->outHeight[i];
      lp = BufDescList[i].bufPlanes[0].width  ;
      cp = lp* (BufDescList[i].bufPlanes[0].height/(n*c)) ;
      void *rtPtr  = ins[i]->ptr;
      void * ivPtr = BufDescList[i].bufPlanes[0].buf;
      offset = lp*ioPrms->outPadT[i] + ioPrms->outPadL[i];
      int32_t layout = ins[i]->layout;

      if(ins[i]->elementType == TIDL_SinglePrecFloat)
      {
        ins[i]->scale = 1.0;
        ins[i]->zeroPoint = 0;
      }

      if(ins[i]->scale == -1)
      {
        ins[i]->scale = scales[i];
      }

      outScale = ins[i]->scale;
      zp = ins[i]->zeroPoint;

      for(i3 = 0; i3 < n; i3++)
      {
      for(i0 = 0; i0 < c; i0++)
      {
        for(i1 = 0; i1 < h; i1++)
        {
          for(i2 = 0; i2 < w; i2++)
          {
            if(ioPrms->outElementType[i] ==  TIDL_UnsignedChar)
            {
              data = ((uint8_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] ==  TIDL_SignedChar)
            {
              data = ((int8_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] ==  TIDL_UnsignedShort)
            {
              data = ((uint16_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] ==  TIDL_SignedShort)
            {
              data = ((int16_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] == TIDL_SinglePrecFloat)
            {
              data = ((float*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] == TIDL_UnsignedWord)
            {
              data = ((uint32_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] == TIDL_SignedWord)
            {
              data = ((int32_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }            
            else if(ioPrms->outElementType[i] == TIDL_UnsignedDoubleWord)
            {
              data = ((uint64_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }
            else if(ioPrms->outElementType[i] == TIDL_SignedDoubleWord)
            {
              data = ((int64_t*)ivPtr)[offset + i3*cp*c + i0*cp + i1*lp + i2];
            }            
            else
            {
              TIDLTB_ASSERT_MSG(0);
            }
            if((outScale != scales[i]) || (zp != 0))
            {
              data = (((data/scales[i]) * outScale) + zp);
            }

            if(layout == TIDLRT_LT_NCHW)
            {
              idx =  i3*h*w*c + i0*h*w + i1*w + i2;
            }
            else
            {
              idx =  i3*h*w*c + i0 + i1*w*c + i2*c;
            }

            if(ins[i]->elementType ==  TIDLRT_Uint8)
            {
              ((uint8_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType ==  TIDLRT_Int8)
            {
              ((int8_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType ==  TIDLRT_Uint16)
            {
              ((uint16_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType ==  TIDLRT_Int16)
            {
              ((int16_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType == TIDLRT_Float32)
            {
              ((float*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType == TIDLRT_Uint32)
            {
              ((uint32_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType == TIDLRT_Int32)
            {
              ((int32_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType == TIDLRT_Uint64)
            {
              ((uint64_t*)rtPtr)[idx] = data;
            }
            else if(ins[i]->elementType == TIDLRT_Int64)
            {
              ((int64_t*)rtPtr)[idx] = data;
            }
            else
            {
              TIDLTB_ASSERT_MSG(0);
            }
          }
        }
      }
      }
    }
  }
}

int32_t TIDL_disableInterrupts()
{
  int32_t oldIntState = 0;
  oldIntState = HwiP_disable();   /* openWearable: was commented out (no-op) */
  return oldIntState;  
}
void TIDL_restoreInterrupts(int32_t oldIntState)
{
  /* openWearable: production-faithful restore (mirror vx_tidl_target.c TIDL_unlockInterrupts).
   * The stock TEST harness had HwiP_restore commented out AND deliberately memset 0xDA over
   * DMEM0/DMEM1/SARAM0 (= L1/L2/L3 scratch) to validate engine reload across critical sections;
   * that corrupts the LIVE MMA/DRU scratch mid-algProcess and stalls the DRU. Removed. */
  HwiP_restore(oldIntState);
}

extern void * gUdmaDrvObjPtr;
int32_t TIDLRT_create(sTIDLRT_Params_t *prms, void ** handle)
{
  int32_t numMemRec, status, i;
  uint32_t reservedCtrl;
  IALG_MemRec *memRec;
  #ifdef TIDL_UC_AVV_BUILD
    volatile uint32_t* syncWritePtr_UC_CLR = (uint32_t*)(MULTICORE_SYNC_BASE) + C7_0_WR_OFFSET;
    *(syncWritePtr_UC_CLR) = 0x0;
  #endif
  /*-----------------------------------------------------------------
  Pointer to algorithm instance
  -----------------------------------------------------------------*/
  IM_Fxns * alHandle = NULL;
  (*(volatile unsigned int*)0xb8000080u)=0xe0e00000u; /*OW create marker*/ 

  sTIDLRTTB_IntHandle_t * rtHandle = (sTIDLRTTB_IntHandle_t*)malloc(sizeof(sTIDLRTTB_IntHandle_t));
  TIDLTB_ASSERT_MSG(rtHandle != NULL);
  TIDL_CreateParams * createPrms = &(rtHandle->createParams);


  TI_CreateMemoryHandle(&memObj_DMEM0,  L1Scratch, L1_MEM_SIZE);
  TI_CreateMemoryHandle(&memObj_DMEM1,  L2Scratch, L2_MEM_SIZE);
  TI_CreateMemoryHandle(&memObj_SARAM0, L3Scratch, L3_MEM_SIZE);
  TI_CreateMemoryHandle(&memObj_EXTMEM, L4Scratch, L4_MEM_SIZE);

  /*-----------------------------------------------------------------
  Set algorithm parameters
  -----------------------------------------------------------------*/

  TIDL_createParamsInit(createPrms);

  /* this is temprary code would be removed afterfixing the OD layer */
  reservedCtrl   = prms->flowCtrl >> 16;
  prms->flowCtrl = (prms->flowCtrl & 0xFFFF);

  createPrms->visionParams.algParams.size   = sizeof(TIDL_CreateParams);
  createPrms->visionParams.cacheWriteBack   = NULL;
  createPrms->currLayersGroupId             = 1;
  createPrms->isInbufsPaded                 = 1;
  createPrms->optimiseExtMem                = TIDL_OptimiseExtMemL1;
  createPrms->flowCtrl                      = prms->flowCtrl;
  createPrms->reservedCtrl                  = reservedCtrl;
  createPrms->TIDLWriteBinToFile            = prms->TIDLWriteBinToFile;
  createPrms->TIDLReadBinFromFile           = prms->TIDLReadBinFromFile;
  createPrms->TIDL_CustomLayerProcess       = TIDL_customLayerProcess;

  createPrms->TIDLVprintf = tidl_tb_utils_printf;

#if HOST_EMULATION
  createPrms->pFxnLock                      = NULL ;
  createPrms->pFxnUnLock                    = NULL ;
#else
#ifdef TIDL_UC_AVV_BUILD
  createPrms->pFxnLock                      = NULL;
  createPrms->pFxnUnLock                    = NULL;
#else
  createPrms->pFxnLock                      = TIDL_disableInterrupts;
  createPrms->pFxnUnLock                    = TIDL_restoreInterrupts;
#endif
#endif
  createPrms->traceLogLevel                 = prms->traceLogLevel;
  createPrms->traceWriteLevel               = prms->traceWriteLevel;
  createPrms->dumpNetInitBackupData         = prms->dumpNetInitBackupData;
  strncpy(createPrms->traceBaseName, prms->traceBaseName, TIDL_STRING_SIZE);
  createPrms->quantRangeExpansionFactor     = prms->quantRangeExpansionFactor;
  createPrms->quantRangeUpdateFactor        = prms->quantRangeUpdateFactor ;
  createPrms->maxPreEmptDelay               = prms->maxPreEmptDelay ;
  createPrms->udmaDrvObj                    = gUdmaDrvObjPtr;
  createPrms->targetPriority                = prms->targetPriority;
  createPrms->net = (sTIDL_Network_t *)prms->netPtr;
  if ((prms->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    createPrms->net->dataFlowInfo = NULL;
  }


  numMemRec = TIDL_VISION_FXNS.ialg.algNumAlloc();
  memRec    = (IALG_MemRec *)malloc(numMemRec*sizeof(IALG_MemRec));
  if(memRec)
  {    
    tidl_tb_printf(2, "Calling algAlloc\n");
    status = TIDL_VISION_FXNS.ialg.algAlloc((IALG_Params *)createPrms, NULL, memRec);
    (*(volatile unsigned int*)0xb8000084u)=0xe0e00001u; /*OW create marker*/ (*(volatile unsigned int*)0xb8000088u)=(unsigned int)status;     
    TIDLTB_ASSERT_MSG(status == IALG_EOK);
    if(status != IALG_EOK)
    {
      tidl_printStatus(status);
    }
  }
  else
  {
    status = IALG_EFAIL;
  }

  if(status == IALG_EOK)
  {
    for(i=0; i<numMemRec; i++)
    {
      if(memRec[i].space == IALG_DARAM1) /* L2 SRAM*/
      {
        if(memRec[i].size > L2_MEM_SIZE)
        {
          tidl_tb_printf(0, "TB is used to configure caches as 64 KB so cannot use more than 448 KB of SRAM, current requested size is %d\n", memRec[i].size);
          status = IALG_EFAIL;
        }
        else
        {
          break;
        }
      }
    }
  }
  if(status == IALG_EOK)
  {
    /*-----------------------------------------------------------------
    Allocate memory for all the mem records
    Application has to implement the below function
    Refer the example implementation below in this file
    -----------------------------------------------------------------*/
    status = TestApp_AllocMemRecords(prms, memRec,numMemRec);
    (*(volatile unsigned int*)0xb800008cu)=0xe0e00002u; (*(volatile unsigned int*)0xb8000090u)=(unsigned int)status; /*OW create marker*/
    TIDLTB_ASSERT_MSG(status == IALG_EOK);
    if(status != IALG_EOK)
    {
      tidl_printStatus(status);
    }
  }
  if(status == IALG_EOK)
  {
    alHandle = (IM_Fxns *) memRec[0].base;
    (*(volatile unsigned int*)0xb8000094u)=0xe0e00003u; /*OW create marker*/
    status = TIDL_VISION_FXNS.ialg.algInit((IALG_Handle)(alHandle),
                        memRec,NULL,(IALG_Params *)(createPrms));
    (*(volatile unsigned int*)0xb8000098u)=0xe0e00004u; (*(volatile unsigned int*)0xb800009cu)=(unsigned int)status; /*OW create marker*/

    if(status != IALG_EOK)
    {
      tidl_tb_printf(0, "Algorithm Init failed with error number: %d\n", status);
    }
    else
    {
      rtHandle->algHandle = alHandle;
      rtHandle->memRec = memRec;
      rtHandle->rtPrms = *prms;
      rtHandle->procIdx = 0;
      *handle = rtHandle;

    }
  }

  return status;
}

volatile uint32_t * gDebugCtrlSetIteration = (volatile uint32_t *)(TIDL_DEBUG_CTRL_BASE + TIDL_DEBUG_CTRL_SET_ITERATION_OFFSET);

extern void *gPrivArgs ;
int32_t TIDLRT_invoke(void *handle, sTIDLRT_Tensor_t *in[], sTIDLRT_Tensor_t *out[])
{

  sTIDLRTTB_IntHandle_t *rtHandle = (sTIDLRTTB_IntHandle_t *)handle;
  int32_t status = IALG_EOK, i;
  int32_t frameIdx = 0;
  IM_Fxns *algHandle = rtHandle->algHandle;
  /*-----------------------------------------------------------------
  Algorithm parameters memory in stack
  -----------------------------------------------------------------*/

  TIDL_InArgs inArgs;
  IVISION_InBufs inBufs;
  /*-----------------------------------------------------------------
  For test purpose it is allocated in stack. Application may get
  this descriptors from its buffer manager
  -----------------------------------------------------------------*/
  IVISION_BufDesc inBufDesc[TIDL_MAX_ALG_IN_BUFS];
  IVISION_BufDesc outBufDesc[TIDL_MAX_ALG_OUT_BUFS];
  IVISION_BufDesc *inBufDescList[TIDL_MAX_ALG_IN_BUFS];

  TIDL_outArgs outArgs;
  IVISION_OutBufs outBufs;
  sTIDLRT_Params_t *prms = &(rtHandle->rtPrms);
  sTIDL_IOBufDesc_t *ioBufDescPtr = (sTIDL_IOBufDesc_t *)prms->ioBufDescPtr;
  /*-----------------------------------------------------------------
  For test purpose it is allocated in stack. Application may get
  this descriptors from its buffer manager
  -----------------------------------------------------------------*/
  IVISION_BufDesc *outBufDescList[TIDL_MAX_ALG_OUT_BUFS];
  int32_t iterCount = 1;
  int32_t iter;
  inBufs.bufDesc = inBufDescList;
  outBufs.bufDesc = outBufDescList;

  inBufs.numBufs = tidl_AllocNetInputMem(ioBufDescPtr, inBufDesc);
  outBufs.numBufs = tidl_AllocNetOutputMem(ioBufDescPtr, outBufDesc);
  
  if ((inBufs.numBufs == 0) || (outBufs.numBufs == 0))
  {
    status = IALG_EFAIL;
  }
  if (status == IALG_EOK)
  {
    for (i = 0; i < inBufs.numBufs; i++)
    {
      inBufDescList[i] = &inBufDesc[i];
    }
    for (i = 0; i < outBufs.numBufs; i++)
    {
      outBufDescList[i] = &outBufDesc[i];
    }

    outArgs.iVisionOutArgs.size = sizeof(TIDL_outArgs);
    inArgs.iVisionInArgs.size = sizeof(TIDL_InArgs);
    inArgs.iVisionInArgs.subFrameInfo = 0;
#ifdef HOST_EMULATION
    inArgs.enableLayerPerfTraces = 0;
#else
    inArgs.enableLayerPerfTraces = (prms->traceLogLevel > 0);
#endif
    if(!(TIDL_FLOW_CTRL_REF_STAT & prms->flowCtrl))
    {
      if (rtHandle->procIdx == 0)
      {
        if ((prms->flowCtrl & 1) == 1)
        {
          tidl_tb_printf(0, " ----------------------- TIDL Process with REF_ONLY FLOW------------------------ \n");
        }
        else
        {
          tidl_tb_printf(0, " ----------------------- TIDL Process with TARGET DATA FLOW ------------------------\n");
        }
      }

      tidl_tb_printf(0, "\n#%5d", rtHandle->procIdx);
      tidl_tb_printf(0, " .");
      // algHandle->ivision->ialg.algActivate((IALG_Handle)(algHandle));
      tidl_tb_printf(0, " ..");
      rtHandle->procIdx++;
    }
    else
    {
      /*Calibration progress:*/
      tidl_tb_progressBar((float)((float)(rtHandle->procIdx + 1)/gParams.numFrames));
      rtHandle->procIdx++;
    }
  }

  /* Copy TIDL RT Tensor to ivision Buffer */
  tidl_copyRTTensorToInBufs((const sTIDL_Network_t *)prms->netPtr, (sTIDL_IOBufDesc_t *)prms->ioBufDescPtr, in, inBufDesc);
  /*Function to set/generate static input buffers (Section in program) */
  tidl_genSetStaticInput((sTIDL_IOBufDesc_t *)prms->ioBufDescPtr, inBufDesc);
  
#ifdef TIDL_PRESILICON_SDK_UC_ENABLE
  *gDebugCtrlSetIteration = 1;  
  while(1)
#endif  
  {
#ifdef TIDL_PRESILICON_SDK_UC_ENABLE
    iterCount = *gDebugCtrlSetIteration;
#endif    
    int32_t isMatching = 0;
    tidl_wait_for_uc_start();
    if (status == IALG_EOK)
    {
      tidl_ddrStatsReadCounters(&ddr_read, &ddr_write);
      tidl_profileMarkerStart();      
      (*(volatile unsigned int*)0xb80000c0u)=0xf0f00001u; /*OW invoke probe pre-process*/
      Cache_wbInvAll(); /*OW cache flush: make CPU-written weights/input/TRs visible to the DRU*/
      tidl_write_frameHeader(frameIdx, 1);

      
      for ( iter = 0; iter < iterCount; iter++)
      {
        status = algHandle->ivision->algProcess((IVISION_Handle)algHandle,
                                                &inBufs, &outBufs, (IVISION_InArgs *)&inArgs, (IVISION_OutArgs *)&outArgs);
      }
      
      (*(volatile unsigned int*)0xb80000c4u)=0xf0f00002u; (*(volatile unsigned int*)0xb80000c8u)=(unsigned int)status; /*OW invoke probe post-process*/
      gPrivArgs = outArgs.privArgs ;
      tidl_write_frameHeader(frameIdx, 2);

      tidl_profileMarkerEnd();
      if(!(TIDL_FLOW_CTRL_REF_STAT & prms->flowCtrl))
      {
        tidl_printProfileInfo();
      }
      tidl_ddrStatsReadCounters(&ddr_read, &ddr_write);
      tidl_printDdrStats(ddr_read, ddr_write);

      TIDLTB_ASSERT_MSG(status == IALG_EOK);
      if (status != IALG_EOK)
      {
        tidl_printStatus(status);
        goto Exit;
      }
  }
    if (status == IALG_EOK)
    {
      /* Copy ivision Buffer to TIDL RT Tensor */
      int32_t isComparisonRequired = tidl_is_comparison_required();
      // This should be the comparison point for static builds
      // Copy op is irrelavant for -> Need to not alloc for OutBufs..
      if (isComparisonRequired == 1)
      {
        isMatching = tidl_genCompStaticOutput((sTIDL_IOBufDesc_t *)prms->ioBufDescPtr, outBufDesc);
      }      
    }
    tidl_write_frameInfo(frameIdx, gFinalCycles, ddr_read /* DDR_READ*/, ddr_write /* DDR_WRITE*/, isMatching);

    if (status == IALG_EOK)
    {
      tidl_copyOutBufsToRTTensor((sTIDL_IOBufDesc_t *)prms->ioBufDescPtr, out, outBufDesc, outArgs.scale);
    }
    frameIdx++;
    if ( frameIdx % 1000 == 0)
    {
      frameIdx = 0;
    }
    tidl_wait_for_uc_end();
  }
  
Exit:
  if(!(TIDL_FLOW_CTRL_REF_STAT & prms->flowCtrl))
  {
    tidl_tb_printf(0, " ....");
  }
  tidl_FreeNetInputMem(ioBufDescPtr, inBufDesc);
  tidl_FreeNetOutputMem(ioBufDescPtr, outBufDesc);
  if(!(TIDL_FLOW_CTRL_REF_STAT & prms->flowCtrl))
  {
    tidl_tb_printf(0, " .....");
  }

  return status;
}

int32_t TIDLRT_delete(void *handle)
{
  sTIDLRTTB_IntHandle_t * rtHandle = (sTIDLRTTB_IntHandle_t*)handle;
  int32_t  status = IALG_EOK;
  IM_Fxns *algHandle = rtHandle->algHandle;
  IALG_MemRec *memRec = rtHandle->memRec;
  int32_t numMemRec = TIDL_VISION_FXNS.ialg.algNumAlloc();
  status = algHandle->ivision->ialg.algFree((IALG_Handle)(algHandle), memRec);
  TIDLTB_ASSERT_MSG(status == IALG_EOK);
  if(status != IALG_EOK)
  {
    tidl_printStatus(status);
  }
  if(status == IALG_EOK)
  {
    sTIDLRT_Params_t * prms = &(rtHandle->rtPrms);
    /* Here Free memory for all the mem records */
    status = TestApp_FreeMemRecords(prms, memRec,numMemRec);
    TIDLTB_ASSERT_MSG(status == IALG_EOK);
    if(status != IALG_EOK)
    {
      tidl_printStatus(status);
    }
  }
  free(memRec);
  free(handle);
  return status;
}

int32_t TIDLRT_deactivate(void *handle)
{
  sTIDLRTTB_IntHandle_t * rtHandle = (sTIDLRTTB_IntHandle_t*)handle;
  int32_t  status = IALG_EOK;
  IM_Fxns *algHandle = rtHandle->algHandle;
  algHandle->ivision->ialg.algDeactivate((IALG_Handle)(algHandle));
  return status;
}
