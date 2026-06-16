/* Copyright 2026 Jonathan Realmuto

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

// Board-specific definitions for the AM335x (BeagleBone) target.
// They are declared (extern) in pru.h, so the rest of the A8 library stays
// board-agnostic and pru.c can be kept identical across branches.
#include "pru.h"

// Remoteproc symlinks under /dev/remoteproc (kernel
// CONFIG_REMOTEPROC_CDEV + BeagleBone udev) for the two PRU cores.
const char* const rp_pru0 = "/dev/remoteproc/pruss-core0";
const char* const rp_pru1 = "/dev/remoteproc/pruss-core1";

// PRU firmware images are named "<pru_fw_prefix>-pru{0,1}-<app>-fw"
// in /lib/firmware.
const char* const pru_fw_prefix = "am335x";
