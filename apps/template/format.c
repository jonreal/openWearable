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
#include <stdint.h>

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
          "%u\t"        // pru0 var - uint32_t
          "%u\t"        // pru1 var - uint32_t
          "%u\t"        // cpu var - uint32_t
          "\n",
          st->time,
          st->pru0var,
          st->pru1var,
          st->cpudata.cpuvar
          );
}

void FormatSprintStateHeader(char* buff) {
  sprintf(buff,
          "\n# frame\t"
          "pru0val\t"
          "pru1val\t"
          "cpuval\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st,buff);
}

// ---------------------------------------------------------------------------
//  Binary log schema (template)
//
//  One record = the four state fields packed little-endian, 16 bytes:
//    time:u32, pru0var:u32, pru1var:u32, cpuvar:u32
//  Keep FormatLogSchema's field list in lock-step with FormatLogRecord so the
//  host-side decoder can rebuild the columns from the file header.
// ---------------------------------------------------------------------------
int FormatLogRecordBytes(void) {
  return 4 * (int)sizeof(uint32_t);
}

void FormatLogSchema(char* buff) {
  sprintf(buff,
          "\n#fields: time:u32,pru0var:u32,pru1var:u32,cpuvar:u32\n"
          "#record_bytes: %d\n",
          FormatLogRecordBytes());
}

void FormatLogRecord(const state_t* st, void* rec) {
  uint32_t* r = (uint32_t*) rec;
  r[0] = st->time;
  r[1] = st->pru0var;
  r[2] = st->pru1var;
  r[3] = st->cpudata.cpuvar;
}
