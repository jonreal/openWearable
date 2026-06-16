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
#include <stdio.h>



const ow_field_t ow_schema[] = {
  { "frame",   OW_U32, offsetof(state_t, frame)           },
  { "pressure", OW_FIX16, offsetof(state_t, pressure)       },
  { "sync", OW_U32, offsetof(state_t, sync_state)         },
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
