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
          "%u\t"        // cpu var - uint32_t
          "%f\t"        // pru1Var - fix16_t
          "%u\t"        // pru2Var - uint32_t
          "\n",
          st->time,
          st->cpudata.cpuVar,
          fix16_to_float(st->pruVar1),
          st->pruVar2
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "cpuVar\t"
          "pru1Var\t"
          "pru2Var\t"
          "\n");
}

// Variables to 'publish' over udp or ROS
void FormatSprintPublishState(const state_t* st, char* buff) {
  sprintf(buff,
          "%u\t"        // timeStamp - uint32_t
          "%u\t"        // cpu var - uint32_t
          "%f\t"        // pru1Var - fix16_t
          "%u\t"        // pru2Var - uint32_t
          "\n",
          st->time,
          st->cpudata.cpuVar,
          fix16_to_float(st->pruVar1),
          st->pruVar2
          );
}
