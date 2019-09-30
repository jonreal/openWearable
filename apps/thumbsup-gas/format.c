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
          "%u\t"        // sync - uint32_t
          "%f\t"        // cpudata - imu1.x - fix16_t
          "%f\t"        // cpudata - imu1.y - fix16_t
          "%f\t"        // cpudata - imu1.z - fix16_t
          "%u\t"        // imu status - uint32_t
          "\n",
          st->time,
          st->sync,
          fix16_to_float(st->cpudata.imu1.x),
          fix16_to_float(st->cpudata.imu1.y),
          fix16_to_float(st->cpudata.imu1.z),
          st->cpudata.status
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "sync\t"
          "imu_x\t"
          "imu_y\t"
          "imu_z\t"
          "imu_status\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  sprintf(buff,
          "%u\t"        // timeStamp - uint32_t
          "%u\t"        // sync - int32_t
          "%i\t"        // buttons - int32_t
          "\n",
          st->time,
          st->sync,
          0
          );
}