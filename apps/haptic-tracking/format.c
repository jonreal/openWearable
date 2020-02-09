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
          "%f\t"        // xd - fix16_t
          "%u\t"        // sync - uint32_t
          "%f\t"        // motor u - fix16_t
          "%f\t"        // motor current - fix16_t
          "%f\t"        // motor vel - fix16_t
          "%f\t"        // dx - fix16_t
          "%f\t"        // dx - fix16_t
          "\n",
          st->time,
          fix16_to_float(st->x),
          fix16_to_float(st->xd),
          st->vsync,
          fix16_to_float(st->motor.u),
          fix16_to_float(st->motor.current),
          fix16_to_float(st->motor.velocity),
          fix16_to_float(st->dx),
          fix16_to_float(st->tau_active)
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "x\t"
          "xd\t"
          "sync\t"
          "u\t"
          "current\t"
          "velocity\t"
          "dx\t"
          "tau_active\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st, buff);
//    sprintf(buff,
//            "%u\t"        // timeStamp - uint32_t
//            "\n",
//            st->time
//            );
}
