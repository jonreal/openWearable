/* Copyright 2017-2019 Jonathan Realmuto

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


#include "cpuloop.h"
#include <stdio.h>
#include "rc_i2c.h"
#include "bno055_arm.h"

imu_t* imu1;
uint32_t frame;

void CpuInit(cpudata_t* cpudata) {
  cpudata->varcpu = 0;
}

void CpuLoop(cpudata_t* cpudata)  {
  (cpudata->varcpu)++;
}

void CpuCleanup(void) {

}
