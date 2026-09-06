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

// Fault codes. Grow per client (SPI/ADC/...) as they adopt the framework.
typedef enum {
  ERR_NONE = 0,
  ERR_I2C_TIMEOUT,
  ERR_I2C_NACK,
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

// PRU-side. Bind the shared-memory hooks once, from the framework init:
//   fault      - the shared fault record (setting .raised IS the fail-stop signal)
//   frame_src  - a live tick counter, copied into the record on raise
void ErrorInit(volatile error_t* fault,
               const volatile uint32_t* frame_src);

// PRU-side. Latch the first fault and request the coordinated fail-stop
// (clears enable so both cores exit through the normal cleanup/vent path).
void ErrorRaise(error_core_t core, error_code_t code, uint32_t context);

#endif /* _ERROR_H_ */
