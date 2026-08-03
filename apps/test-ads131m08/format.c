/* test-ads131m08 log schema -- the 8 ADS131M08 channels (read by the R5F over
 * McSPI7), plus the R5F loop counter, the ADC response word, and the McSPI7
 * HL_REV reach self-check. See apps/template/format.c for the schema notes.
=============================================================================*/

#include "format.h"
#include <stddef.h>
#include <stdio.h>

const ow_field_t ow_schema[] = {
  { "frame",  OW_U32, offsetof(state_t, frame)              },
  { "r5fvar", OW_U32, offsetof(state_t, r5f_state.r5fvar)   },
  { "adc0",   OW_I32, offsetof(state_t, r5f_state.adc[0])   },
  { "adc1",   OW_I32, offsetof(state_t, r5f_state.adc[1])   },
  { "adc2",   OW_I32, offsetof(state_t, r5f_state.adc[2])   },
  { "adc3",   OW_I32, offsetof(state_t, r5f_state.adc[3])   },
  { "adc4",   OW_I32, offsetof(state_t, r5f_state.adc[4])   },
  { "adc5",   OW_I32, offsetof(state_t, r5f_state.adc[5])   },
  { "adc6",   OW_I32, offsetof(state_t, r5f_state.adc[6])   },
  { "adc7",   OW_I32, offsetof(state_t, r5f_state.adc[7])   },
  { "resp",   OW_U32, offsetof(state_t, r5f_state.response) },
  { "hlrev",  OW_U32, offsetof(state_t, r5f_state.hlrev)    },
};
const size_t ow_schema_count = sizeof ow_schema / sizeof ow_schema[0];

void FormatSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n"
          "#\tR5F decimate = %i (ADC read every Nth PRU frame)\n#",
          param->fs_hz, param->r5f_decimate);
}
