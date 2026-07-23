/* test-pams log schema -- PAM pressure + valve command, plus the R5F counter
 * (to watch the per-run reset). See apps/template/format.c for the schema notes.
=============================================================================*/

#include "format.h"
#include <stddef.h>
#include <stdio.h>

const ow_field_t ow_schema[] = {
  { "frame",  OW_U32,   offsetof(state_t, frame)             },
  { "p_res",  OW_FIX16, offsetof(state_t, p_res)             },
  { "pm_raw", OW_FIX16, offsetof(state_t, pam_state.pm_raw)  },
  { "pm",     OW_FIX16, offsetof(state_t, pam_state.pm)      },
  { "pd",     OW_FIX16, offsetof(state_t, pam_state.pd)      },
  { "u",      OW_I32,   offsetof(state_t, pam_state.u)       },
  { "r5fvar", OW_U32,   offsetof(state_t, r5f_state.r5fvar)  },
};
const size_t ow_schema_count = sizeof ow_schema / sizeof ow_schema[0];

void FormatSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n"
          "#\tPd = %3.2f\n"
          "#\tR5F decimate = %i (hooks every Nth PRU frame)\n#",
          param->fs_hz, fix16_to_float(param->Pd), param->r5f_decimate);
}
