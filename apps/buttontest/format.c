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
          "%d\t"        // fsm_state - int32_t
          "%f\t"        // pm_pro - fix16_t
          "%f\t"        // pd_pro - fix16_t
          "%f\t"        // pm_sup - fix16_t
          "%f\t"        // pd_sup - fix16_t
          "%u\t"        // sync - uint32_t
          "\n",
          st->time,
          (int32_t) st->fsm_state,
          fix16_to_float(st->pm_pro),
          fix16_to_float(st->pd_pro),
          fix16_to_float(st->pm_sup),
          fix16_to_float(st->pd_sup),
          st->sync
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "fsm_state\t"
          "pm_pro\t"
          "pd_pro\t"
          "pm_sup\t"
          "pd_sup\t"
          "sync\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
