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

// PRU-side fault reporting. Deliberately depends only on error.h (not
// mem_types.h) so it builds into libow_pru.a without an app-specific state.h.

#include <stdint.h>
#include "error.h"

// Consecutive dropped sensor reads tolerated before fail-stop (2 => one retry).
const uint32_t error_max_consecutive_drops = 2;

static volatile error_t*        g_fault  = 0;
static const volatile uint32_t* g_frame  = 0;

void ErrorInit(volatile error_t* fault,
               const volatile uint32_t* frame_src) {
  g_fault  = fault;
  if (fault) fault->raised = 0u;   // start clean regardless of stale SRAM
  g_frame  = frame_src;
}

void ErrorRaise(error_core_t core, error_code_t code, uint32_t context) {
  if (g_fault && !g_fault->raised) {        // fail-stop: keep the FIRST fault
    g_fault->frame   = g_frame ? *g_frame : 0u;
    g_fault->context = context;
    g_fault->code    = (uint16_t) code;
    g_fault->core    = (uint8_t)  core;
    g_fault->raised  = 1u;
  }
  // Setting fault.raised (above) IS the fail-stop signal: both PRU loops test it
  // in while(RUN && !fault.raised) and fall through to cleanup()/vent; the A8
  // sees it too. No enable/shared-bit write -> stays within owned-words rules.
}
