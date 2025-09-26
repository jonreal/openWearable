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

// Constant Table
volatile far pruIntc
  CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg
  CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep
  CT_IEP __attribute__((cregister("PRU_IEP", near), peripheral));

// Pointers to start of shared memory sections defined in .cmd
extern far char __PRU_PARAM;
extern far char __PRU_LUTAB;
extern far char __PRU_SHAREDMEM;

// Main ----------------------------------------------------------------------
int main(void) {
  pru_count_t counter = {0, 0};
  pru_mem_t mem = {NULL, NULL, NULL};

  initialize(&mem);

  // wait till enabled
  while (mem.s->pru_ctl.bit.enable == 0);
  mem.s->pru_ctl.bit.shdw_enable = mem.s->pru_ctl.bit.enable;
  clearIepInterrupt();
  startTimer();

  // Control Loop
  while (mem.s->pru_ctl.bit.shdw_enable) {

    // Poll for IEP timer interrupt
    while ((CT_INTC.SECR0 & (1 << 7)) == 0);

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

  // ensure nothing hangs
  mem.s->pru_ctl.bit.pru0_done = 0;
  mem.s->pru_ctl.bit.pru1_done = 0;

  cleanup();
  __halt();
  return 0;
}

// ----------------------------------------------------------------------------
void initialize(pru_mem_t* mem) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // PRU CTRL: CTR_EN = 0x1 - enable cycle counter
  HWREG(PRU_CTRL_BASE) |= (1 << 3);

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

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
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  // drivers
  AdcCleanup();

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // user defined cleanups
  Pru0Cleanup();
}
void memInit(pru_mem_t* mem) {

  // Memory map for shared memory
  mem->s = (shared_mem_t*) &__PRU_SHAREDMEM;

  // Memory map for parameters (pru0 DRAM)
  mem->p = (param_mem_t*) &__PRU_PARAM;

  // Memory map for feedforward lookup table (pru1 DRAM)
  mem->l = (lut_mem_t*) &__PRU_LUTAB;

  // Point global debug buffer
  debug_buff = &(mem->p->debug_buff[0]);
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
  // Enable ocp_clk
  CT_CFG.IEPCLK_bit.OCP_EN = 1;

  // Disable counter
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  // Reset Count register
  CT_IEP.TMR_CNT = 0x0;

  // Clear overflow status register
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  // Set compare value
  CT_IEP.TMR_CMP0 = (freq_counts-1);

  // Clear compare status
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  // Disable compensation
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  // Enable CMP0 and reset on event
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;
}

void iepInterruptInit(void) {
  /*** System event 7 Interrupt -> Host 1 ***/

  // Disable Global enable interrupts
  CT_INTC.GER = 0;

  // Clear any pending PRU-generated events
  __R31 = 0x00000000;

  // Map event 7 to channel 2
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  // Map Channel 2 to host 2
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  // Clear the status of all interrupts
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  // Enable event 7
  CT_INTC.EISR = 7;

  // Enable Host interrupt 1
  CT_INTC.HIEISR |= (1 << 0);

  // Gloablly enable interrupts
  CT_INTC.GER = 1;
}

void startTimer(void) {
  CT_IEP.TMR_GLB_CFG = 0x11;
}

void clearTimerFlag(void) {
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
}

void clearIepInterrupt(void) {
    CT_INTC.SECR0 = (1<<7);
    __R31 = 0x00000000;
}
