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
          "%f\t"        // p1m_raw - fix16_t
          "%f\t"        // p1m - fix16_t
          "%f\t"        // p1d - fix16_t
          "%d\t"        // u1 - int32_t
          "%f\t"        // p2m_raw - fix16_t
          "%f\t"        // p2m - fix16_t
          "%f\t"        // p2d - fix16_t
          "%d\t"        // u2 - int32_t
          "%f\t"        // p3m_raw - fix16_t
          "%f\t"        // p3m - fix16_t
          "%f\t"        // p3d - fix16_t
          "%d\t"        // u3 - int32_t
          "%f\t"        // p4m_raw - fix16_t
          "%f\t"        // p4m - fix16_t
          "%f\t"        // p4d - fix16_t
          "%d\t"        // u4 - int32_t
          "%f\t"        // trigger signal - fix16_t
          "\n",
          st->time,
          fix16_to_float(st->p_res),
          fix16_to_float(st->pam1_state.pm_raw),
          fix16_to_float(st->pam1_state.pm),
          fix16_to_float(st->pam1_state.pd),
          st->pam1_state.u,
          fix16_to_float(st->pam2_state.pm_raw),
          fix16_to_float(st->pam2_state.pm),
          fix16_to_float(st->pam2_state.pd),
          st->pam2_state.u,
          fix16_to_float(st->pam3_state.pm_raw),
          fix16_to_float(st->pam3_state.pm),
          fix16_to_float(st->pam3_state.pd),
          st->pam3_state.u,
          fix16_to_float(st->pam4_state.pm_raw),
          fix16_to_float(st->pam4_state.pm),
          fix16_to_float(st->pam4_state.pd),
          st->pam4_state.u,
          fix16_to_float(st->triggersignal)
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "p_res\t"
          "p1m_raw\t"
          "p1m\t"
          "p1d\t"
          "u1\t"
          "p2m_raw\t"
          "p2m\t"
          "p2d\t"
          "u2\t"
          "p3m_raw\t"
          "p3m\t"
          "p3d\t"
          "u3\t"
          "p4m_raw\t"
          "p4m\t"
          "p4d\t"
          "u4\t"
          "triggersignal\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
