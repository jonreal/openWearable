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
#include "udf.h"

// Prototypes -----------------------------------------------------------------
void initialize(pru_mem_t* pru_mem);
void cleanup(void);
void memInit(pru_mem_t* pru_mem);
void updateCounters(uint32_t* cnt, uint32_t* si);
void debugPinHigh(void);
void debugPinLow(void);
void iepTimerInit(uint32_t count);
void iepInterruptInit(void);
void startTimer(void);
void clearTimerFlag(void);
void clearIepInterrupt(void);

// Globals -------------------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Constant Table
volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

// Debug Buffer
volatile uint32_t *debug_buff;

// Main ----------------------------------------------------------------------
int main(void) {
  uint32_t cnt = 0;
  uint32_t si = 0;
  pru_mem_t m;

  initialize(&m);

  // wait till enabled
  while (m.s->pru_ctl.bit.enable == 0);
  m.s->pru_ctl.bit.shdw_enable = m.s->pru_ctl.bit.enable;
  clearIepInterrupt();
  startTimer();

  // Control Loop
  while (m.s->pru_ctl.bit.shdw_enable) {

    // Poll for IEP timer interrupt
    while ((CT_INTC.SECR0 & (1 << 7)) == 0);

    // Pre bookkeeping
    clearTimerFlag();
    debugPinHigh();
    m.s->pru_ctl.bit.shdw_enable = m.s->pru_ctl.bit.enable;
    m.s->state[si].time_stamp = cnt;

    // Estimate
    Pru0UpdateState(cnt, si, &m);

    // Wait for pru1 to be done
    m.s->pru_ctl.bit.pru0_done = 1;
    while(!(m.s->pru_ctl.bit.pru1_done));
    m.s->pru_ctl.bit.pru1_done = 0;

    // Control
    Pru0UpdateControl(cnt, si, &m);

    // Post bookkeeping
    m.s->cbuff_index = si;
    updateCounters(&cnt, &si);
    clearIepInterrupt();
    debugPinLow();
 }
  debugPinLow();
  cleanup();
  __halt();
  return 0;
}

// ----------------------------------------------------------------------------
void initialize(pru_mem_t* pru_mem) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // PRU CTRL: CTR_EN = 0x1 - enable cycle counter
  HWREG(PRU_CTRL_BASE) |= (1 << 3);

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

  memInit(pru_mem);

  // Init timer
  iepInterruptInit();
  iepTimerInit(pru_mem->p->fs_ticks);
  clearIepInterrupt();

  // drivers
  adcInit();
  i2cInit();

  // user defined inits
  Pru0Init();
}

void cleanup(void) {
  // Clear all interrupts
  clearIepInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  // drivers
  adcCleanup();
  i2cCleanUp();

  // user defined cleanups
  Pru0Cleanup();
}

void memInit(pru_mem_t* pru_mem) {
  // Memory map for shared memory
  void* ptr = (void*) PRU_L_SHARED_DRAM;
  pru_mem->s = (shared_mem_t*) ptr;

  // Memory map for parameters (pru0 DRAM)
  ptr = (void*) PRU_DRAM;
  pru_mem->p = (param_mem_t*) ptr;

  // Memory map for feedforward lookup table (pru1 DRAM)
  ptr = (void*) PRU_OTHER_DRAM;
  pru_mem->l = (lookUp_mem_t*) ptr;

  // Point global debug buffer
  debug_buff = &(pru_mem->p->debug_buff[0]);
}

void updateCounters(uint32_t* cnt, uint32_t* si)
{
  (*cnt)++;
  (*si)++;
  (*si) %= STATE_BUFF_LEN;
}

void debugPinHigh(void)
{
  __R30 |= (1 << PRU0_DEBUG_PIN);
}

void debugPinLow(void)
{
  __R30 &= ~(1 << PRU0_DEBUG_PIN);
}

void iepTimerInit(uint32_t count)
{
  // Enable ocp_clk
  CT_CFG.IEPCLK_bit.OCP_EN = 1;

  // Disable counter
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  // Reset Count register
  CT_IEP.TMR_CNT = 0x0;

  // Clear overflow status register
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  // Set compare value
  CT_IEP.TMR_CMP0 = (count-1);

  // Clear compare status
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  // Disable compensation
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  // Enable CMP0 and reset on event
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;
}

void iepInterruptInit(void)
{
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

void startTimer(void)
{
  // Start Timer
  CT_IEP.TMR_GLB_CFG = 0x11;
}

void clearTimerFlag(void)
{
  // Clear compare status
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
}

void clearIepInterrupt(void)
{
    // Clear interrupt status
    CT_INTC.SECR0 = (1<<7);
    __R31 = 0x00000000;
}
