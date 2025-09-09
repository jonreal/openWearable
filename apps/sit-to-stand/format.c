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

void FormatSprintParams(const param_mem_t* param, char* buff) {
  sprintf(buff, "\n#Parameters:\n"
          "#\tFs = %i (Hz)\n#",
          param->fs_hz);
}

// ---------------------------------------------------------------------------
// Function: void printState(uint8_t si, FILE *fp)
//           void sprintState(uint8_t si, char* buff)
//
//  This function prints the states.
//
// Input: pointer to file or buff
// ---------------------------------------------------------------------------
void FormatSprintState(const state_t* st, char* buff) {
  sprintf(buff,
          "%u\t"        // timeStamp - uint32_t
          "%f\t"        // p_res - fix16_t
          "%f\t"        // pm_raw - fix16_t
          "%f\t"        // pm - fix16_t
          "%f\t"        // pd - fix16_t
          "%d\t"        // u - int32_t
          "%d\t"        // load_r - int32_t
          "%d\t"        // load_l - int32_t
          "%u\t"        // sync - uint32_t
          "%u\t"        // status - uint32_t
          "\n",
          st->time,
          fix16_to_float(st->p_res),
          fix16_to_float(st->pam_state.pm_raw),
          fix16_to_float(st->pam_state.pm),
          fix16_to_float(st->pam_state.pd),
          st->pam_state.u,
          st->load_r,
          st->load_l,
          st->sync_state,
          st->status
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "p_res\t"
          "pm_raw\t"
          "pm\t"
          "pd\t"
          "u\t"
          "load_r\t"
          "load_l\t"
          "sync\t"
          "status\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
