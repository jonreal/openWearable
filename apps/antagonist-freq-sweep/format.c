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
          "%f\t"        // x - fix16_t
          "%f\t"        // motor u - fix16_t
          "%u\t"        // pwmcmp - uint32_t
          "%f\t"        // motor current - fix16_t
          "%f\t"        // motor vel - fix16_t
          "%f\t"        // pam1 pm_raw - fix16_t
          "%f\t"        // pam1 pm - fix16_t
          "%f\t"        // pam1 pd - fix16_t
          "%i\t"        // pam1 u - uint8_t
          "%f\t"        // pam2 pm_raw - fix16_t
          "%f\t"        // pam2 pm - fix16_t
          "%f\t"        // pam2 pd - fix16_t
          "%i\t"        // pam2 u - uint8_t
          "%u\t"        // sync - uint32_t
          "\n",
          st->time,
          fix16_to_float(st->x),
          fix16_to_float(st->motor.u),
          st->motor.pwmcmpvalue,
          fix16_to_float(st->motor.current),
          fix16_to_float(st->motor.velocity),
          fix16_to_float(st->pam1_state.pm_raw),
          fix16_to_float(st->pam1_state.pm),
          fix16_to_float(st->pam1_state.pd),
          st->pam1_state.u,
          fix16_to_float(st->pam2_state.pm_raw),
          fix16_to_float(st->pam2_state.pm),
          fix16_to_float(st->pam2_state.pd),
          st->pam2_state.u,
          st->sync
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "x\t"
          "u\t"
          "pwmcmp\t"
          "current\t"
          "velocity\t"
          "pm1_raw\t"
          "pm1\t"
          "pd1\t"
          "u1\t"
          "pm2_raw\t"
          "pm2\t"
          "pd2\t"
          "u2\t"
          "sync\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st, buff);
}
