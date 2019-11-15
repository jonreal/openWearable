/* Copyright 2018 Jonathan Realmuto

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


#include "emg.h"
#include <stdlib.h>
#include "spidriver.h"

extern volatile uint32_t *debug_buff;

// using mcp6008 over spi for adc

emg_t* EmgInitSensor(uint8_t chan, iir_filt_t* hp, iir_filt_t* lp) {
  emg_t* emg = malloc(sizeof(emg_t));
  emg->adc_ch = chan;
  emg->s.raw = 0;
  emg->s.value = 0;
  emg->hp = hp;
  emg->lp = lp;
  return emg;
}

void EmgCleanup(emg_t* emg) {
  free(emg);
}

void EmgUpdate(emg_t* emg) {
  uint32_t tx = (0x3 << 22) | (emg->adc_ch << 19);
  uint32_t rx = spiXfer(0x0, tx);

  emg->s.raw = fix16_from_int( ((int)(((rx & 0x1FFFF) >> 7) & 0x3FF)));

  // Note that emg is down sampled so filter must take account of that
  emg->s.value = FiltIir(abs(FiltIir(emg->s.raw,emg->hp)),emg->lp);

}

emg_state_t EmgGetState(emg_t* emg) {
  return emg->s;
}
