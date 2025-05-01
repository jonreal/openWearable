/* Copyright 2017 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>
#include "rsc_table_pru0.h"
#include "mem_types.h"
#include "hw_types.h"
#include "adcdriver.h"
#include "i2cdriver.h"
#include "spidriver.h"
#include "pruloop.h"

// Globals (pru_io) -----------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;
volatile uint32_t* debug_buff;

// Prototypes -----------------------------------------------------------------
void initialize(pru_mem_t* mem);
void cleanup(void);
void memInit(pru_mem_t* mem);
void updateCounters(pru_count_t* c);
void debugPinHigh(void);
void debugPinLow(void);
void iepTimerInit(uint32_t freq_counts);
void iepInterruptInit(void);
void startTimer(void);
void clearTimerFlag(void);
void clearIepInterrupt(void);

#if AM335x
  volatile far pruIntc CT_INTC
    __attribute__((cregister("PRU_INTC", far), peripheral));
  volatile pruCfg CT_CFG
    __attribute__((cregister("PRU_CFG", near), peripheral));
  volatile far pruIep CT_IEP
    __attribute__((cregister("PRU_IEP", near), peripheral));
  volatile __far ctrl CT_PRU0_CTRL
    __attribute__((cregister("PRU0_CTRL", near), peripheral));
#else
  volatile far intc CT_INTC
    __attribute__((cregister("PRU_INTC", far), peripheral));
  volatile cfg CT_CFG
    __attribute__((cregister("PRU_CFG", near), peripheral));
  volatile far iep CT_IEP0
    __attribute__((cregister("PRU_IEP0", near), peripheral));
#endif


// Main ----------------------------------------------------------------------
int main(void) {
  pru_count_t counter = {0, 0};
  pru_mem_t mem = {NULL, NULL, NULL};

  //volatile uint32_t tmp = CT_IEP0.count_reg0;  // read test

  initialize(&mem);

  // wait till enabled
  while (mem.s->pru_ctl.bit.enable == 0);
  mem.s->pru_ctl.bit.shdw_enable = mem.s->pru_ctl.bit.enable;
  clearIepInterrupt();
  startTimer();

  // Control Loop
  while (mem.s->pru_ctl.bit.shdw_enable) {

    // Poll for IEP timer interrupt
    // am335x
    //while ((CT_INTC.SECR0 & (1 << 7)) == 0);

    //am64x
    while ((CT_INTC.RAW_STATUS_REG0 & (1 << 7)) == 0);

    // Pre bookkeeping
    clearTimerFlag();
    debugPinHigh();
    mem.s->pru_ctl.bit.shdw_enable = mem.s->pru_ctl.bit.enable;
    mem.s->state[counter.index].time = counter.frame;

    // Estimate
    Pru0UpdateState(
      &counter,
      mem.p,
      mem.l,
      &mem.s->state[counter.index],
      &mem.s->pru_ctl
    );

    // Wait for pru1 to be done
    mem.s->pru_ctl.bit.pru0_done = 1;
    debugPinLow();
    while(!(mem.s->pru_ctl.bit.pru1_done));
    debugPinHigh();
    mem.s->pru_ctl.bit.pru1_done = 0;

    // Control
    Pru0UpdateControl(
      &counter,
      mem.p,
      mem.l,
      &mem.s->state[counter.index],
      &mem.s->pru_ctl
    );

    // Copy cpudata to state
    mem.s->state[counter.index].cpudata = mem.s->cpudata;

    // Post bookkeeping
    mem.s->cbuff_index = counter.index;
    updateCounters(&counter);
    clearIepInterrupt();
    debugPinLow();
 }
  debugPinLow();
  cleanup();
  __halt();
  return 0;
}

// ----------------------------------------------------------------------------
void initialize(pru_mem_t* mem) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  // am335x
  //CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
//  CT_INTC.STATUS_CLR_INDEX_REG_bit.STATUS_CLR_INDEX = 21;

  // PRU CTRL: CTR_EN = 0x1 - enable cycle counter
  //HWREG(PRU_CTRL_BASE) |= (1 << 3);

  // set PRU0 GP MUX to default (all muxed to PRU)
  CT_CFG.gpcfg0_reg_bit.pr1_pru0_gp_mux_sel = 0;

  // enable IEP OCP clock (needed for timers)
  CT_CFG.iepclk_reg_bit.iep_ocp_clk_en = 1;

  // Release PRU0 from reset
  CT_PRU0_CTRL.CONTROL_bit.SOFT_RST_N = 1;

  // Enable PRU core (PDSP_ENABLE = Processor Enable)
  CT_PRU0_CTRL.CONTROL_bit.PDSP_ENABLE = 1;

  // Optional: Enable cycle counter for performance monitoring
  CT_PRU0_CTRL.CONTROL_bit.COUNTER_ENABLE = 1;

  memInit(mem);

  // Init timer
  iepInterruptInit();
  iepTimerInit(mem->p->fs_ticks);
  clearIepInterrupt();

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // drivers
  AdcInit();
  spiInit();

  // user defined inits
  Pru0Init(mem);
}

void cleanup(void) {
  // Clear all interrupts
  clearIepInterrupt();

  // am335x
  //CT_INTC.SECR0 = 0xFFFFFFFF;
  //CT_INTC.SECR1 = 0xFFFFFFFF;

  // am64x
  CT_INTC.STATUS_CLR_INDEX_REG_bit.STATUS_CLR_INDEX = 7;

  // drivers
  AdcCleanup();

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // user defined cleanups
  Pru0Cleanup();
}
void memInit(pru_mem_t* mem) {

  //*shared = 0xAAAAAAAFA;
  //volatile uint32_t* shared = (volatile uint32_t*) (shared_mem_start);
  //uintptr_t base = 0x00010000;
  //uintptr_t param_ram_offset = 0xE000;
  //uintptr_t lut_ram_offset = 0xF000;

  mem->s = (shared_mem_t*) sharedram_base;
  mem->s->state[0].time = 0xDEADDEAD;

  volatile uint32_t*
      param_mem = (volatile uint32_t*) (sharedram_base + param_ram_offset);
  param_mem[0] = 0xAFAF;

  //mem->l = (lut_mem_t*) (base + lut_ram_offset);

  //debug_buff = &(mem->p->debug_buff[0]);
  //debug_buff[0] = 0xF0F;

  //// Memory map for parameters
  //mem->p = (param_mem_t*) (param_mem_start);

  //// Memory map for LUT
  //mem->l = (lut_mem_t*) (lut_mem_start);

  //// Point global debug buffer
  //debug_buff = &(mem->p->debug_buff[0]);

  //// TEST
  //mem->s->state[0].time = 0xDEADBEAF;
  //debug_buff[9] = 0xFAEBDAED;
  //debug_buff[0] = 0xFAEBDAED;
  //debug_buff[5] = 0xFAEBDAED;

}

void updateCounters(pru_count_t* c) {
  (c->frame)++;
  (c->index)++;
  (c->index) %= STATE_BUFF_LEN;
}

void debugPinHigh(void) {
  __R30 |= (1 << PRU0_DEBUG_PIN);
}

void debugPinLow(void) {
  __R30 &= ~(1 << PRU0_DEBUG_PIN);
}

void iepTimerInit(uint32_t freq_counts) {
  //am355x
  // Enable ocp_clk
  //CT_CFG.IEPCLK_bit.OCP_EN = 1;

  //// Disable counter
  //CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  //// Reset Count register
  //CT_IEP.TMR_CNT = 0x0;

  //// Clear overflow status register
  //CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  //// Set compare value
  //CT_IEP.TMR_CMP0 = (freq_counts-1);

  //// Clear compare status
  //CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  //// Disable compensation
  //CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  //// Enable CMP0 and reset on event
  //CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  //CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;

  //am64x
  // Disable counter
  CT_IEP0.global_cfg_reg_bit.cnt_enable = 0;

  // Reset Count reg
  CT_IEP0.count_reg0 = 0x0;
  CT_IEP0.count_reg1 = 0x0;

  // Clear overflow status reg
  CT_IEP0.global_status_reg_bit.cnt_ovf = 1;

  // Set compare value
  CT_IEP0.cmp0_reg0 = freq_counts - 1;

  // Clear compare status
  CT_IEP0.cmp_status_reg = 0xFF;

  // Disable compensation
  CT_IEP0.compen_reg_bit.compen_cnt = 0x0;

  // Enable cmp0 and reset on event
  CT_IEP0.cmp_cfg_reg_bit.cmp0_rst_cnt_en = 0;
  CT_IEP0.cmp_cfg_reg_bit.cmp_en = 1;
}

void iepInterruptInit(void) {
  /*** System event 7 Interrupt -> Host 1 ***/


  // am335x
  //// Disable Global enable interrupts
  //CT_INTC.GER = 0;

  //// Clear any pending PRU-generated events
  //__R31 = 0x00000000;

  //// Map event 7 to channel 2
  //CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  //// Map Channel 2 to host 2
  //CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  //// Clear the status of all interrupts
  //CT_INTC.SECR0 = 0xFFFFFFFF;
  //CT_INTC.SECR1 = 0xFFFFFFFF;

  //// Enable event 7
  //CT_INTC.EISR = 7;

  //// Enable Host interrupt 1
  //CT_INTC.HIEISR |= (1 << 0);

  //// Gloablly enable interrupts
  //CT_INTC.GER = 1;

  //am64x
  // Clear any pending PRU-generated events
  __R31 = 0x00000000;

  // Map event 7 to channel 2
  CT_INTC.CH_MAP_REG1_bit.CH_MAP_7 = 1;

  // Map Channel 2 to host 2
  CT_INTC.HINT_MAP_REG0_bit.HINT_MAP_1 = 1;

  // Enable event 7
  CT_INTC.ENABLE_REG0_bit.ENABLE_7 = 1;

  // Enable Host interrupt 1
  CT_INTC.ENABLE_HINT_REG0_bit.ENABLE_HINT_1 = 1;

  // Global Enable
  CT_INTC.GLOBAL_ENABLE_HINT_REG_bit.ENABLE_HINT_ANY = 1;

}

void startTimer(void) {
  // am335x
  //CT_IEP.TMR_GLB_CFG = 0x11;
  //am64x
  CT_IEP0.global_cfg_reg = 0x11;
}

void clearTimerFlag(void) {
  // am335x
  //CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
  // am64x
  CT_IEP0.cmp_status_reg_bit.cmp_status = 0xFF;
}

void clearIepInterrupt(void) {
    //CT_INTC.SECR0 = (1<<7);
    //__R31 = 0x00000000;
    CT_INTC.STATUS_CLR_INDEX_REG_bit.STATUS_CLR_INDEX = 7;
    __R31 = 0x00000000;
}
