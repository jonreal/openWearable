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

#ifndef _NNLOOP_H_
#define _NNLOOP_H_

#include "mem_types.h"   // brings nn_feat_t / nn_out_t via state.h

// Per-app NN hooks -- one set per app (like pruloop/cpuloop/uiloop). Called by
// the inference thread in library/a72/src/nn.c. An app opts into C7x inference
// by providing these + the -n/-i model flags.

// One-time setup before the worker thread starts (warmup, scaling constants).
void NnInit(void);

// The "forward method": pack the model input from a window over mem->s->state[]
// (plus any preprocessing). Output is float (A72 has an FPU). Called once per
// inference, on the worker thread.
void NnForward(const pru_mem_t* mem, nn_feat_t* feat);

// Optional: app-side use of a fresh result (already published to cpu_state.nn).
void NnInterpret(const nn_out_t* out);

// One-time teardown after the worker thread stops.
void NnCleanup(void);

#endif /* _NNLOOP_H_ */
