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
          "#\tFs = %i\n"
          "#\tdP = %f\n"
          "#\tthreshold = %f\n"
          "#\temg_up_threshold = %f\n"
          "#\temg_low_threshold = %f\n#",
          param->fs_hz,
          fix16_to_float(param->dP),
          fix16_to_float(param->threshold),
          fix16_to_float(param->emg_up_thres),
          fix16_to_float(param->emg_low_thres));
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
          "%f\t"        // p_res - fix16_t
          "%f\t"        // pam1 pm_raw - fix16_t
          "%f\t"        // pam1 pm - fix16_t
          "%f\t"        // pam1 pd - fix16_t
          "%i\t"        // pam1 u - uint8_t
          "%f\t"        // pam2 pm_raw - fix16_t
          "%f\t"        // pam2 pm - fix16_t
          "%f\t"        // pam2 pd - fix16_t
          "%i\t"        // pam2 u - uint8_t
          "%f\t"        // trigger signal - fix16_t
          "%f\t"        // trigger signalmyo - fix16_t
          "%f\t"        // emg1 raw signal - fix16_t
          "%f\t"        // emg1 signal - fix16_t
          "%f\t"        // emg2 raw signal - fix16_t
          "%f\t"        // emg2 signal - fix16_t
          "\n",
          st->time,
          fix16_to_float(st->x),
          fix16_to_float(st->xd),
          st->vsync,
          fix16_to_float(st->motor.u),
          fix16_to_float(st->motor.current),
          fix16_to_float(st->motor.velocity),
          fix16_to_float(st->dx),
          fix16_to_float(st->tau_active),
          fix16_to_float(st->p_res),
          fix16_to_float(st->pam1_state.pm_raw),
          fix16_to_float(st->pam1_state.pm),
          fix16_to_float(st->pam1_state.pd),
          st->pam1_state.u,
          fix16_to_float(st->pam2_state.pm_raw),
          fix16_to_float(st->pam2_state.pm),
          fix16_to_float(st->pam2_state.pd),
          st->pam2_state.u,
          fix16_to_float(st->triggersignal),
          fix16_to_float(st->triggersignal_myo),
          fix16_to_float(st->emg1_state.raw),
          fix16_to_float(st->emg1_state.value),
          fix16_to_float(st->emg1_state.raw),
          fix16_to_float(st->emg2_state.value)
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
          "p_res\t"
          "pm1_raw\t"
          "pm1\t"
          "pd1\t"
          "u1\t"
          "pm2_raw\t"
          "pm2\t"
          "pd2\t"
          "u2\t"
          "triggersignal\t"
          "triggersignal_myo\t"
          "emg1_raw\t"
          "emg1\t"
          "emg2_raw\t"
          "emg2\t"
          "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
//  FormatSprintState(st, buff);
    sprintf(buff,
            "%u\t"        // timeStamp - uint32_t
            "%f\t"
            "%f\t"
            "%i\t"
            "\n",
            st->time,
            fix16_to_float(st->x),
            fix16_to_float(st->xd),
            st->game
            );
}
