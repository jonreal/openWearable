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
          "%f\t"        // reservior pressure - fix16_t
          "%f\t"        // pam1 pm - fix16_t
          "%f\t"        // pam1 pd - fix16_t
          "%i\t"        // pam1 u - uint8_t
          "%f\t"        // pam2 pm - fix16_t
          "%f\t"        // pam2 pd - fix16_t
          "%i\t"        // pam2 u - uint8_t
          "\n",
          st->time,
          fix16_to_float(st->p_res),
          fix16_to_float(st->pam1_state.p_m),
          fix16_to_float(st->pam1_state.p_d),
          st->pam1_state.u,
          fix16_to_float(st->pam2_state.p_m),
          fix16_to_float(st->pam2_state.p_d),
          st->pam2_state.u
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "p_res\t"
          "p_m1\t"
          "p_d1\t"
          "u1\t"
          "p_m2\t"
          "p_d2\t"
          "u1\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
