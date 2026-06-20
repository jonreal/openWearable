/* Copyright 2018 Jonathan Realmuto

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

// Board-specific definitions for the J721E / TDA4VM (BeagleBone AI-64) target.
// This is the single place the SoC differs from the am335x backend: the two PRU
// remoteproc node paths and the firmware-image prefix. They are declared
// (extern) in pru.h, so the rest of the A72 library stays board-agnostic and
// pru.c is byte-identical to main's (aside from PruMemMap). The am335x board.c
// is the same shape with /dev/remoteproc/pruss-core{0,1} and the "am335x" prefix.

#include "pru.h"

// remoteproc nodes for the two ICSSG PRU cores (named symlinks under
// /dev/remoteproc). pru.c builds "<path>/firmware" and "<path>/state" from these.
const char* const rp_pru0 = "/dev/remoteproc/j7-pru0_0";
const char* const rp_pru1 = "/dev/remoteproc/j7-pru0_1";

// Firmware images: "<pru_fw_prefix>-pru{0,1}-<app>-fw<pru_fw_ext>" in /lib/firmware
// (convention j7-<proc>-<app>-fw.ow; .ow marks openWearable vs stock TI firmware).
const char* const pru_fw_prefix = "j7";
const char* const pru_fw_ext = ".ow";

// R5F remoteproc node (main_r5fss0, lockstep -> one rproc = core0). The app loads OW
// control firmware "j7-r5f0-<app>-fw.ow" onto it at runtime (R5fInit), like the PRUs.
const char* const rp_r5f0 = "/dev/remoteproc/j7-main-r5f0_0";
