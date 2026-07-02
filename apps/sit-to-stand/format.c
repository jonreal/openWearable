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
//  Supported types: OW_{U,I}{8,16,32,64}, OW_F{32,64}, OW_FIX16 (Q16.16, stored
//  raw, decoded to float by utils/logdecode).
// ---------------------------------------------------------------------------
const ow_field_t ow_schema[] = {
  { "frame",  OW_U32,   offsetof(state_t, frame)            },
  { "p_res",  OW_FIX16, offsetof(state_t, p_res)            },
  { "pm_raw", OW_FIX16, offsetof(state_t, pam_state.pm_raw) },
  { "pm",     OW_FIX16, offsetof(state_t, pam_state.pm)     },
  { "pd",     OW_FIX16, offsetof(state_t, pam_state.pd)     },
  { "u",      OW_I32,   offsetof(state_t, pam_state.u)      },
  { "load_r", OW_I32,   offsetof(state_t, load_r)           },
  { "load_l", OW_I32,   offsetof(state_t, load_l)           },
  { "sync",   OW_U32,   offsetof(state_t, sync_state)       },
  { "status", OW_U32,   offsetof(state_t, status)           },
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
