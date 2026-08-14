/* Copyright 2017-2026 Jonathan Realmuto

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
#include "ads131.h"
#include "sync.h"

#define ADS131_CS_PIN 6   /* P9.27 */
#define SYNC_IN_PIN   5

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// ID register captured at init (0x28xx = ADS131M08 present and out of reset).
// Stamped into every state record so a bad init is visible in the log.
static uint16_t ads_id;

sync_t* sync;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  // CLKIN must already be running (started in ARM space -- see uiloop.c), or
  // the RESET below leaves the part with no modulator clock and every frame
  // comes back as the same post-reset acknowledgment word.
  const ads131_cfg_t cfg = { ADS131_CS_PIN };
  ads_id = Ads131Init(&cfg);
}

void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {
  int32_t ch[ADS131_NCH];
  uint32_t i;

  Ads131ReadAllChannels(ch);

  for (i = 0; i < ADS131_NCH; i++)
    io->s->adc[i] = ch[i];

  io->s->response = Ads131LastStatus();
  io->s->id = ads_id;
}

void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io) {
  debug_buff[0] = 0xFF;
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
  sync = SyncInitChan(SYNC_IN_PIN);
}

void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {
  SyncInUpdate(sync);
  io->s->sync_state = SyncGetState(sync);
}

void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {
}

void Pru1Cleanup(void) {
}
