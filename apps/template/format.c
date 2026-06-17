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
//  state_t is for shared access, not just logging, so list only the fields you
//  want, in any order. Supported types: OW_{U,I}{8,16,32,64}, OW_F{32,64},
//  OW_FIX16 (Q16.16, stored raw, decoded to float by utils/logdecode).
// ---------------------------------------------------------------------------
const ow_field_t ow_schema[] = {
  { "frame",   OW_U32, offsetof(state_t, frame)          },
  { "pru0var", OW_U32, offsetof(state_t, pru0var)        },
  { "pru1var", OW_U32, offsetof(state_t, pru1var)        },
  { "cpuvar",  OW_U32, offsetof(state_t, cpudata.cpuvar) },
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
