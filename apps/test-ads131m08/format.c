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
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "%u\t"        // rx - uint32_t
          "\n",
          st->time,
          st->rx0,
          st->rx1,
          st->rx2,
          st->rx3,
          st->rx4,
          st->rx5,
          st->rx6,
          st->rx7,
          st->rx8,
          st->rx9
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "rx0\t"
          "rx1\t"
          "rx2\t"
          "rx3\t"
          "rx4\t"
          "rx5\t"
          "rx6\t"
          "rx7\t"
          "rx8\t"
          "rx9\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}
