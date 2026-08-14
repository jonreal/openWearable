/* Copyright 2018-2026 Jonathan Realmuto

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

#include "format.h"
#include <stddef.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
//  Log schema -- the one thing you edit per app.
//
//  One row per logged column: { name, type, offsetof(state_t, field) }. The
//  generic core (library/a8/src/format_core.c) turns this single table into the
//  packed binary record, the '#fields:' file header, and the -v debug screen.
//
//  adc0..adc7 are raw sign-extended 24-bit ADS131M08 codes; convert on the host
//  (volts = code / 2^23 * VREF / gain). Column names match the am64x branch's
//  apps/test-ads131m08 so one decoder/analysis script serves both.
// ---------------------------------------------------------------------------
const ow_field_t ow_schema[] = {
  { "frame", OW_U32, offsetof(state_t, frame)      },
  { "adc0",  OW_I32, offsetof(state_t, adc[0])     },
  { "adc1",  OW_I32, offsetof(state_t, adc[1])     },
  { "adc2",  OW_I32, offsetof(state_t, adc[2])     },
  { "adc3",  OW_I32, offsetof(state_t, adc[3])     },
  { "adc4",  OW_I32, offsetof(state_t, adc[4])     },
  { "adc5",  OW_I32, offsetof(state_t, adc[5])     },
  { "adc6",  OW_I32, offsetof(state_t, adc[6])     },
  { "adc7",  OW_I32, offsetof(state_t, adc[7])     },
  { "resp",  OW_U32, offsetof(state_t, response)   },
  { "id",    OW_U32, offsetof(state_t, id)         },
  { "sync",  OW_U32, offsetof(state_t, sync_state) },
};
const size_t ow_schema_count = sizeof ow_schema / sizeof ow_schema[0];

// ---------------------------------------------------------------------------
//  Parameters header (app-specific: param_mem_t differs per app).
// ---------------------------------------------------------------------------
void FormatSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n#",
          param->fs_hz);
}
