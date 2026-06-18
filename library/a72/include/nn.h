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

#ifndef _NN_H_
#define _NN_H_

#include "mem_types.h"

// A72 <-> C7x TIDL inference plumbing (generic; the per-app feature/result
// logic lives in the NnForward()/NnInterpret() hooks, see nnloop.h).
//
// NnStart() opens the C7x DDR mailbox (tidl/src/ow_c7x_tidl.c protocol), stages
// the compiled net + io descriptor, calls the app NnInit(), and spawns a
// free-running worker thread. Each cycle the thread calls NnForward() to pack
// features, runs one C7x invoke, converts the float output to fix16, and
// publishes it into pru_mem->s->cpudata.nn (which PRU0 snapshots every tick).
//
// Returns 0 on success; on failure the mailbox is torn down and inference is
// simply not running (the rest of the app is unaffected).
int  NnStart(pru_mem_t* pru_mem, const char* net_path, const char* io_path);

// Stop the worker thread, call the app NnCleanup(), and release the mailbox.
void NnStop(void);

#endif /* _NN_H_ */
