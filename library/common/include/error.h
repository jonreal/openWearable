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

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdint.h>

// Which core raised the fault.
typedef enum {
  ERR_CORE_PRU0,
  ERR_CORE_PRU1,
  ERR_CORE_ARM,
} error_core_t;

// Fault codes. Grow per client as they adopt the framework. APPEND ONLY --
// pam.c casts i2c_t.last_err (ERR_I2C_*) through this enum, so existing values
// must not shift.
typedef enum {
  ERR_NONE = 0,
  ERR_I2C_TIMEOUT,
  ERR_I2C_NACK,
  ERR_ADC_TIMEOUT,
  ERR_SPI_TIMEOUT,
  ERR_ENC_TIMEOUT,
  ERR_PRU_PEER_TIMEOUT,
} error_code_t;

// Single latched fault record (fail-stop: the first fault wins). Lives in
// shared memory so the ARM can read why the run ended.
typedef struct {
  volatile uint32_t frame;    // tick when raised
  volatile uint32_t context;  // aux: (mux_channel << 8) | i2c_address, etc.
  volatile uint16_t code;     // error_code_t
  volatile uint8_t  core;     // error_core_t
  volatile uint8_t  raised;   // 0 until set; ARM watches this
} error_t;

// Consecutive dropped sensor reads tolerated before fail-stop (2 => one retry).
// Defined in error.c; promotable to a runtime param later.
extern const uint32_t error_max_consecutive_drops;

// PRU-side. Bind the shared-memory hooks once, from each core's pru_main:
//   self       - THIS core's id (ERR_CORE_PRU0 / ERR_CORE_PRU1). The PRUs are
//                interchangeable, so core identity is injected here (the one place
//                it's known) rather than hardcoded in the shared drivers.
//   fault      - the shared fault record (setting .raised IS the fail-stop signal)
//   frame_src  - a live tick counter, copied into the record on raise
void ErrorInit(error_core_t self,
               volatile error_t* fault,
               const volatile uint32_t* frame_src);

// PRU-side, core-agnostic. Latch the first fault and request the coordinated
// fail-stop (both cores exit through the normal cleanup/vent path). The raising
// core is the one bound in ErrorInit -- callers never pass it.
void ErrorRaise(error_code_t code, uint32_t context);

#endif /* _ERROR_H_ */
