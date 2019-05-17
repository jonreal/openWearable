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
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg1 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // emg2 - fix16_t
          "%f\t"        // p1_m - fix16_t
          "%f\t"        // p1_d - fix16_t
          "%f\t"        // p2_m - fix16_t
          "%f\t"        // p2_d - fix16_t
          "%i\t"        // u1 - int16_t
          "%i\t"        // u2 - int16_t
          "%f\t"        // angle - fix16_t
          "%f\t"        // angle_d - fix16_t
          "%u\t"        // sync_val - uint16_t
          "\n",
          st->time,
          fix16_to_float(st->emg1_raw),
          fix16_to_float(st->emg1_rect),
          fix16_to_float(st->emg1_nlb),
          fix16_to_float(st->emg2_raw),
          fix16_to_float(st->emg2_rect),
          fix16_to_float(st->emg2_nlb),
          fix16_to_float(st->p1_m),
          fix16_to_float(st->p1_d),
          fix16_to_float(st->p2_m),
          fix16_to_float(st->p2_d),
          (int32_t)st->u1,
          (int32_t)st->u2,
          fix16_to_float(st->angle),
          fix16_to_float(st->angle_d),
          st->sync_val
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "emg1_raw\t"
          "emg1_rect\t"
          "emg1_nlb\t"
          "emg2_raw\t"
          "emg2_rect\t"
          "emg2_nlb\t"
          "p1_m\t"
          "p1_d\t"
          "p2_m\t"
          "p2_d\t"
          "u1\t"
          "u2\t"
          "angle\t"
          "angle_d\t"
          "sync_val\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
    sprintf(buff,
            "%u\t"        // timeStamp - uint32_t
            "%f\t"        // angle - fix16_t
            "%f\t"        // angle_d - fix16_t
            "\n",
            st->time,
            fix16_to_float(st->angle),
            fix16_to_float(st->angle_d)
            );
}
