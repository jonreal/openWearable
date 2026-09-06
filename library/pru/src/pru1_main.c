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

// Constant Table
volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

// Pointers to start of shared memory defined in .cmd
extern far char __PRU_PARAM;
extern far char __PRU_LUTAB;
extern far char __PRU_SHAREDMEM;

// Deadline for the PRU<->PRU seq barrier (must match pru0_main): a peer that
// stops advancing its seq fail-stops instead of hanging us. See
// notes/comm-hardening-plan.md (Item 4).
static const uint32_t PEER_WAIT_BUDGET = 2000000u;

// Main ----------------------------------------------------------------------
int main(void) {
  pru_count_t counter = {0, 0};
  pru_mem_t mem = {NULL, NULL, NULL};

  initialize(&mem);

  // fault reporting: bind the shared fault record, the enable bit, and the
  // live tick counter so ErrorRaise() can fail-stop from deep in the drivers.
  ErrorInit(ERR_CORE_PRU1, &mem.s->fault, &counter.frame);

  // Hook handles: a const view (read-only) + a mutable io surface. The stable
  // pointers are set once here; io.s is repointed to the current slot each tick.
  pru_view_t view = { &counter, mem.p, mem.l };
  pru_io_t   io   = { NULL, &mem.s->arm, &mem.s->pru1 };

  // publish readiness, wait for A8 enable (arm RUN)
  mem.s->pru1_seq = 0;
  mem.s->pru1 = PRU_READY;
  while (!(mem.s->arm & ARM_RUN));
  debugPinLow();

  uint32_t n = 0;

  // Control Loop
  while ((mem.s->arm & ARM_RUN) && !mem.s->fault.raised) {

    // Poll of IEP timer interrupt
    while ((CT_INTC.SECR0 & (1 << 7)) == 0);

    // Pre bookkeeping
    debugPinHigh();
    n++;
    io.s = &mem.s->state[counter.index];          // this tick's slot

    // Estimate
    Pru1UpdateState(&view, &io);

    // Barrier: wait for pru0 state-done (bounded -- wedged peer -> fail-stop).
    debugPinLow();
    uint32_t bw = 0;
    while (((int32_t)(mem.s->pru0_seq - n) < 0)
           && (mem.s->arm & ARM_RUN) && !mem.s->fault.raised) {
      if (++bw > PEER_WAIT_BUDGET) {
        ErrorRaise(ERR_PRU_PEER_TIMEOUT, n);
        break;
      }
    }
    debugPinHigh();
    if (!(mem.s->arm & ARM_RUN) || mem.s->fault.raised)
      break;

    // Control
    Pru1UpdateControl(&view, &io);

    // Post bookkeeping (publish control-done seq)
    mem.s->pru1_seq = n;
    updateCounters(&counter);
    while (CT_INTC.SECR0 & (1 << 7));
    debugPinLow();
  }
  debugPinLow();

  // (owned-words handshake is single-writer; nothing to unwind)

  cleanup();
  __halt();
  return 0;
}

// ----------------------------------------------------------------------------
void initialize(pru_mem_t* mem) {
  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

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
  // Memory map for shared memory
  mem->s = (shared_mem_t*) &__PRU_SHAREDMEM;

  // Memory map for parameters
  mem->p = (param_mem_t*) &__PRU_PARAM;

  // Memory map for feedforward lookup table
  mem->l = (lut_mem_t*) &__PRU_LUTAB;

  // Point global debug buffer
  debug_buff = &(mem->p->debug_buff[0]);
}

