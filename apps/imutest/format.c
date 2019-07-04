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
          "%f\t"        // euler.x -fix16_t
          "%f\t"        // euler.y -fix16_t
          "%f\t"        // euler.z -fix16_t
          "%f\t"        // euler.x -fix16_t
          "%f\t"        // euler.y -fix16_t
          "%f\t"        // euler.z -fix16_t
          "\n",
          st->time,
          fix16_to_float(st->euler1.x),
          fix16_to_float(st->euler1.y),
          fix16_to_float(st->euler1.z),
          fix16_to_float(st->euler2.x),
          fix16_to_float(st->euler2.y),
          fix16_to_float(st->euler2.z)
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "imu1_roll\t"
          "imu1_pitch\t"
          "imu1_yaw\t"
          "imu2_roll\t"
          "imu2_pitch\t"
          "imu2_yaw\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
