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
void initialize(void);
void cleanup(void);
void initMemory(void);
void updateCounters(uint32_t *cnt, uint32_t *si);
void debugPinHigh(void);
void debugPinLow(void);

// Globals -------------------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

// State pointer
shared_mem_t *s;

// Param pointer
param_mem_t *p;

// Feedforward lookup table pointer
lookUp_mem_t *l;

// Debug Buffer
volatile uint32_t *debugBuffer;

// Main ----------------------------------------------------------------------
int main(void)
{
  uint32_t cnt = 0;
  uint32_t stateIndx = 0;

  initialize();

  // wait till enabled
  while(s->cntrl_bit.enable == 0){}

  // Control Loop
  while(1){

    // Poll of IEP timer interrupt
    while((CT_INTC.SECR0 & (1 << 7)) == 0);

    // Pre bookkeeping
    debugPinHigh();

    // Estimate
    updateState_pru1(cnt, stateIndx);

    // Wait for pru0 to be done
    debugPinLow();
    while(!(s->cntrl_bit.pru0_done));
    debugPinHigh();
    s->cntrl_bit.pru0_done = 0;

    // Control
    updateControl_pru1(cnt, stateIndx);

    // Post bookkeeping
    s->cntrl_bit.pru1_done = 1;
    updateCounters(&cnt, &stateIndx);
    if(!(s->cntrl_bit.shdw_enable))
        break;
    while(CT_INTC.SECR0 & (1 << 7));
    debugPinLow();
  }
  debugPinLow();
  cleanup();
  __halt();
  return 0;
}

// ----------------------------------------------------------------------------
void initialize(void)
{
  /*** Init Pru ***/

  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

  // Memory
  initMemory();

  // user defined inits
  init_pru1();
}

void cleanup(void)
{
  cleanup_pru1();
}

void updateCounters(uint32_t* cnt, uint32_t* si)
{
  (*cnt)++;
  (*si)++;
  (*si) %= SIZE_STATE_BUFF;
}

void debugPinHigh(void)
{
  __R30 |= (1 << PRU1_DEBUG_PIN);
}

void debugPinLow(void)
{
  __R30 &= ~(1 << PRU1_DEBUG_PIN);
}

void initMemory(void)
{
  void *ptr = NULL;

  // Memory map for shared memory
  ptr = (void *)PRU_L_SHARED_DRAM;
  s = (shared_mem_t *) ptr;

  // Memory map for parameters (pru0 DRAM)
  ptr = (void *) PRU_OTHER_DRAM;
  p = (param_mem_t *) ptr;

  // Memory map for feedforward lookup table (pru1 DRAM)
  ptr = (void *) PRU_DRAM;
  l = (lookUp_mem_t *) ptr;

  // Point global debug buffer
  debugBuffer = &(p->debugBuffer[0]);
}

