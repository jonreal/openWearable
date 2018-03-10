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

#include "rsc_table_pru1.h"
#include "mem_types.h"
#include "hw_types.h"
#include "udf.h"

// Prototypes ----------------------------------------------------------------
void initialize(pru_mem_t* pru_mem);
void cleanup(void);
void memInit(pru_mem_t* pru_mem);
void updateCounters(uint32_t* cnt, uint32_t* si);
void debugPinHigh(void);
void debugPinLow(void);

// Globals -------------------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

// Debug Buffer
volatile uint32_t *debug_buff;

// Main ----------------------------------------------------------------------
int main(void) {
  uint32_t cnt = 0;
  uint32_t si = 0;
  pru_mem_t m;

  initialize(&m);

  // wait till enabled
  while (m.s->pru_ctl.bit.shdw_enable == 0);

  // Control Loop
  while (m.s->pru_ctl.bit.shdw_enable) {

    // Poll of IEP timer interrupt
    while ((CT_INTC.SECR0 & (1 << 7)) == 0);

    // Pre bookkeeping
    debugPinHigh();

    // Estimate
    Pru1UpdateState(cnt, si, &m);

    // Wait for pru0 to be done
    debugPinLow();
    while (!(m.s->pru_ctl.bit.pru0_done));
    debugPinHigh();
    m.s->pru_ctl.bit.pru0_done = 0;

    // Control
    Pru1UpdateControl(cnt, si, &m);

    // Post bookkeeping
    m.s->pru_ctl.bit.pru1_done = 1;
    updateCounters(&cnt, &si);
    while (CT_INTC.SECR0 & (1 << 7));
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

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

  // Memory
  memInit(pru_mem);

  // user defined inits
  Pru1Init();
}

void cleanup(void) {
  Pru1Cleanup();
}

void updateCounters(uint32_t* cnt, uint32_t* si) {
  (*cnt)++;
  (*si)++;
  (*si) %= STATE_BUFF_LEN;
}

void debugPinHigh(void) {
  __R30 |= (1 << PRU1_DEBUG_PIN);
}

void debugPinLow(void) {
  __R30 &= ~(1 << PRU1_DEBUG_PIN);
}

void memInit(pru_mem_t* pru_mem) {
  // Memory map for shared memory
  void* ptr = (void*) PRU_L_SHARED_DRAM;
  pru_mem->s = (shared_mem_t*) ptr;

  // Memory map for parameters (pru0 DRAM)
  ptr = (void*) PRU_OTHER_DRAM;
  pru_mem->p = (param_mem_t*) ptr;

  // Memory map for feedforward lookup table (pru1 DRAM)
  ptr = (void*) PRU_DRAM;
  pru_mem->l = (lookUp_mem_t*) ptr;

  // Point global debug buffer
  debug_buff = &(pru_mem->p->debug_buff[0]);
}

