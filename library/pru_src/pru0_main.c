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

  initialize(&mem);

  // wait till enabled
  while (mem.s->pru_ctl.bit.enable == 0);
  mem.s->pru_ctl.bit.shdw_enable = mem.s->pru_ctl.bit.enable;
  //clearIepInterrupt();

  startTimer();

  // Control Loop
  while (mem.s->pru_ctl.bit.shdw_enable) {

    // Poll for interrupt
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

  // set PRU0 GP MUX to default (all muxed to PRU)
  CT_CFG.gpcfg0_reg_bit.pr1_pru0_gp_mux_sel = 0;

  // enable IEP OCP clock (needed for timers)
  CT_CFG.iepclk_reg_bit.iep_ocp_clk_en = 1;

  // Release PRU0 from reset
  CT_PRU0_CTRL.CONTROL_bit.SOFT_RST_N = 1;

  // Enable PRU core (PDSP_ENABLE = Processor Enable)
  CT_PRU0_CTRL.CONTROL_bit.PDSP_ENABLE = 1;

  // Optional: Enable cycle counter for performance monitoring
  CT_PRU0_CTRL.CONTROL_bit.COUNTER_ENABLE = 0;

  memInit(mem);

  // Init timer
  iepInterruptInit();
  iepTimerInit(mem->p->fs_ticks);
  clearIepInterrupt();

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // drivers
  //AdcInit();
  //spiInit();

  // user defined inits
  Pru0Init(mem);

}

void cleanup(void) {
  // Clear all interrupts
  clearIepInterrupt();

  // am64x
  CT_INTC.STATUS_CLR_INDEX_REG_bit.STATUS_CLR_INDEX = 7;

  // drivers
//  AdcCleanup();

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // user defined cleanups
  Pru0Cleanup();
}
void memInit(pru_mem_t* mem) {
  mem->s = (shared_mem_t*) sharedram_base;
  mem->p = (param_mem_t*) (sharedram_base + param_ram_offset);
  mem->l = (lut_mem_t*) (sharedram_base + lut_ram_offset);
  debug_buff = &(mem->p->debug_buff[0]);

  // Clear clt bits
  mem->s->pru_ctl.reg = 0;

  // Zero debug buffer
  for (uint32_t i = 0; i<10; i++){
    debug_buff[i] = 0x0;
  }
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

  // Disable counter
  CT_IEP0.global_cfg_reg_bit.cnt_enable = 0;

  // Reset Count reg
  CT_IEP0.count_reg0 = 0x0;
  CT_IEP0.count_reg1 = 0x0;

  // Clear overflow status reg
  CT_IEP0.global_status_reg_bit.cnt_ovf = 0;

  // Set compare value
  CT_IEP0.cmp0_reg0 = freq_counts - 1;

  // Clear compare status
  CT_IEP0.cmp_status_reg = 0xFF;

  // Disable compensation
  CT_IEP0.compen_reg_bit.compen_cnt = 0x0;

  // Enable cmp0 and reset on event
  CT_IEP0.cmp_cfg_reg_bit.cmp0_rst_cnt_en = 1;
  CT_IEP0.cmp_cfg_reg_bit.cmp_en = 1;


}

void iepInterruptInit(void) {

  // Map event 7 to channel 1
  //pr0_iep_tim_cap_cmp_pend PRU_ICSSG0 IEP0
  CT_INTC.CH_MAP_REG1_bit.CH_MAP_7 = 1;

  // Map Channel 1 to host 1
  CT_INTC.HINT_MAP_REG0_bit.HINT_MAP_1 = 1;

  // Clear any interrupts
  CT_INTC.ENABLE_CLR_REG0 = 0xFFFFFFFF;
  CT_INTC.ENABLE_CLR_REG1 = 0xFFFFFFFF;
  CT_INTC.ENABLE_CLR_REG2 = 0xFFFFFFFF;
  CT_INTC.ENABLE_CLR_REG3 = 0xFFFFFFFF;
  CT_INTC.ENABLE_CLR_REG4 = 0xFFFFFFFF;

  CT_INTC.STATUS_CLR_INDEX_REG = 7;

  // Enable event 7 and enable clear event 7
  CT_INTC.ENABLE_SET_INDEX_REG = 7;

  // Enable Host interrupt 1
  CT_INTC.HINT_ENABLE_SET_INDEX_REG_bit.HINT_ENABLE_SET_INDEX = 0;

  // Enable
  CT_INTC.GLOBAL_ENABLE_HINT_REG_bit.ENABLE_HINT_ANY = 1;

}

void startTimer(void) {
  CT_IEP0.global_cfg_reg_bit.cnt_enable = 1;
  CT_IEP0.global_cfg_reg_bit.default_inc = 1;
}

void clearTimerFlag(void) {
  CT_IEP0.cmp_status_reg = 0xFF;
}

void clearIepInterrupt(void) {
    CT_INTC.ENABLE_CLR_REG0 = 0xFFFFFFFF;
    __R31 = 0x00000000;
}
