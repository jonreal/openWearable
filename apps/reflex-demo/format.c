/* Copyright 2018-2019 Jonathan Realmuto

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
//  Log schema
// ---------------------------------------------------------------------------
const ow_field_t ow_schema[] = {
  { "frame",    OW_U32,       offsetof(state_t, frame)              },
  { "p_res",    OW_FIX16,  offsetof(state_t, p_res)              },
  { "p1m_raw",  OW_FIX16,  offsetof(state_t, pam1_state.pm_raw)  },
  { "p1m",      OW_FIX16,  offsetof(state_t, pam1_state.pm)      },
  { "p1d",      OW_FIX16,  offsetof(state_t, pam1_state.pd)      },
  { "u1",       OW_I32,    offsetof(state_t, pam1_state.u)       },
  { "p2m_raw",  OW_FIX16,  offsetof(state_t, pam2_state.pm_raw)  },
  { "p2m",      OW_FIX16,  offsetof(state_t, pam2_state.pm)      },
  { "p2d",      OW_FIX16,  offsetof(state_t, pam2_state.pd)      },
  { "u2",       OW_I32,    offsetof(state_t, pam2_state.u)       },
  { "trigger",  OW_FIX16,  offsetof(state_t, triggersignal)      },
  { "pot1",     OW_U32,  offsetof(state_t, pot1)      },
  { "pot2",     OW_U32,  offsetof(state_t, pot2)      },
};
const size_t ow_schema_count = sizeof ow_schema / sizeof ow_schema[0];

void FormatSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n#",
          param->fs_hz);
}
