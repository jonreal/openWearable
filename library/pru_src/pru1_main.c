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
#include "pruloop.h"

// Globals (pru_io) -----------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;
volatile uint32_t* debug_buff;

// Prototypes ----------------------------------------------------------------
void initialize(pru_mem_t* mem);
void cleanup(void);
void memInit(pru_mem_t* mem);
void updateCounters(pru_count_t* c);
void debugPinHigh(void);
void debugPinLow(void);

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
  while (mem.s->pru_ctl.bit.shdw_enable == 0);
  debugPinLow();

  // Control Loop
  while (mem.s->pru_ctl.bit.shdw_enable) {

    // Poll for IEP timer
    while ((CT_IEP0.cmp_status_reg & 0x1) == 0);

    // Pre bookkeeping
    debugPinHigh();

    // Estimate
    Pru1UpdateState(
      &counter,
      mem.p,
      mem.l,
      &mem.s->state[counter.index],
      &mem.s->pru_ctl
    );

    // Wait for pru0 to be done
    debugPinLow();


    while (!(mem.s->pru_ctl.bit.pru0_done));

    debugPinHigh();
    mem.s->pru_ctl.bit.pru0_done = 0;


    // Control
    Pru1UpdateControl(
      &counter,
      mem.p,
      mem.l,
      &mem.s->state[counter.index],
      &mem.s->pru_ctl
    );


    // Post bookkeeping
    mem.s->pru_ctl.bit.pru1_done = 1;
    //updateCounters(&counter);

   // while(CT_INTC.RAW_STATUS_REG0 & (1 << 7));
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
  //CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Pin Mux
  // CT_CFG.GPCFG0 = 0;

  // Memory
  memInit(mem);

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  // user defined inits
  Pru1Init(mem);
}

void cleanup(void) {

  // clear gpio
  __R30 = 0x0;
  __R31 = 0x0;

  Pru1Cleanup();
}

void updateCounters(pru_count_t* c) {
  (c->frame)++;
  (c->index)++;
  (c->index) %= STATE_BUFF_LEN;
}

void debugPinHigh(void) {
  __R30 |= (1 << PRU1_DEBUG_PIN);
}

void debugPinLow(void) {
  __R30 &= ~(1 << PRU1_DEBUG_PIN);
}

void memInit(pru_mem_t* mem) {
  mem->s = (shared_mem_t*) sharedram_base;
  mem->p = (param_mem_t*) (sharedram_base + param_ram_offset);
  mem->l = (lut_mem_t*) (sharedram_base + lut_ram_offset);
  debug_buff = &(mem->p->debug_buff[0]);
}

