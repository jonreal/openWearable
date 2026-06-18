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

// FIRST LIGHT: emit the known test vector (matches tidl/host/ow_c7x_infer.py
// X_DEFAULT) so cpudata.nn.y reproduces the verified reference output
// [0, 0.3083, 0.1019, 0.0454, 0, 0.2057, 0.2034, 0] (max|err| ~0.045, int8).
// Once the loop is proven, swap the body for a real window over state[].
void NnForward(const pru_mem_t* mem, nn_feat_t* feat) {
  (void)mem;
  for (int i = 0; i < N_FEAT; i++)
    feat->x[i] = (float)i / (float)N_FEAT - 0.5f;

  // --- real sensor window (disabled until first light passes) ----------------
  // The newest completed ring slot is mem->s->cbuff_index; walk back N_FEAT-1
  // slots for a time window of one channel, converting fix16 sensor data to
  // float for the model:
  //
  //   uint32_t idx = mem->s->cbuff_index;
  //   for (int i = 0; i < N_FEAT; i++) {
  //     uint32_t slot = (idx + 1 + i) % STATE_BUFF_LEN;   // oldest..newest
  //     feat->x[i] = fix16_to_float(mem->s->state[slot].pru1var);
  //   }
}

void NnInterpret(const nn_out_t* out) {
  // Template: the result is consumed via the log schema (format.c) and the PRU
  // snapshot of cpudata.nn -- nothing extra to do here.
  (void)out;
}

void NnCleanup(void) {
}
