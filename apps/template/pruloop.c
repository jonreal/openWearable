/* Copyright 2017-2019 Jonathan Realmuto

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

#include "pruloop.h"



#include "mcspi_j721e.h"   // J721E McSPI driver (pru0_0 bus = McSPI7)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  // pru0_0 hardware SPI bus = McSPI7 (0x2170000). Bring it up + reach self-check:
  // debug_buff[2] = HL_REV; a sane non-zero value confirms the PRU reached the
  // controller through the interconnect/firewall. (Full-duplex loopback was
  // proven during bring-up -- see docs/COMM-BUS-LAYOUT.md.)
  mcspiInit(MCSPI7_BASE, 8u, 200u);
  debug_buff[2] = mcspiHlRev(MCSPI7_BASE);
}

void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {
  //io->s->pru0var++;
  debug_buff[1]++;

  io->s->pru0var = debug_buff[1];
}

void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io) {
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
  // pru0_1 hardware SPI bus = McSPI6 (0x2160000). Same driver, different base.
  // debug_buff[3] = HL_REV reach self-check.
  mcspiInit(MCSPI6_BASE, 8u, 200u);
  debug_buff[3] = mcspiHlRev(MCSPI6_BASE);
}

void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {

  // Pulse a few pins
  if ((view->c->frame % 50) == 0) {
    __R30 ^= (1 << 0);
    __R30 ^= (1 << 1);
  }
  io->s->pru1var++;
}

void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {
}

void Pru1Cleanup(void) {
}

