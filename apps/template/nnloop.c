/* Copyright 2026 Jonathan Realmuto

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

// ---------------------------------------------------------------------------
// Template NN hooks -- the per-app "forward method" for C7x + MMA inference.
//
// The library (library/a72/src/nn.c) owns the mailbox + the worker thread; this
// file owns only WHAT the model sees and (optionally) how its output is used.
// Enable at runtime with: -n <net.bin> -i <io.bin>.
// ---------------------------------------------------------------------------

#include "nnloop.h"

void NnInit(void) {
  // one-time warmup / scaling-constant setup goes here
}

// Live features from the PRU frame counter (read from the newest state[] slot --
// the real windowing entry point). Each input is a multiple of the frame count
// taken as a fix16 (Q16.16) fraction: a bounded sawtooth that advances with the
// control loop at a per-feature rate, so the C7x output visibly tracks live PRU
// data. Stand-in for real sensor features; MLP size unchanged.
//
// (To feed actual sensor data later, window a channel over the ring instead:
//    uint32_t idx = mem->s->cbuff_index;
//    for (i=0..N_FEAT-1) { slot = (idx + 1 + i) % STATE_BUFF_LEN;       // old->new
//                          feat->x[i] = fix16_to_float(mem->s->state[slot].pru1var); } )
void NnForward(const pru_mem_t* mem, nn_feat_t* feat) {
  uint32_t frame = mem->s->state[mem->s->cbuff_index].frame;
  for (int i = 0; i < N_FEAT; i++) {
    fix16_t f = (fix16_t)((frame * (uint32_t)(i + 1)) & 0xFFFF);  // [0,1) in Q16.16
    feat->x[i] = fix16_to_float(f) - 0.5f;                         // center -> [-0.5, 0.5)
  }
}

void NnInterpret(const nn_out_t* out) {
  // Template: the result is consumed via the log schema (format.c) and the PRU
  // snapshot of cpu_state.nn -- nothing extra to do here.
  (void)out;
}

void NnCleanup(void) {
}
