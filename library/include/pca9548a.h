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

#ifndef _PCA9548A_H_
#define _PCA9548A_H_

#include <stdint.h>
#include "i2cdriver.h"

typedef enum {
  PCA9548 = 0,
  PCA9544 = 1
} i2cmux_version_t;

typedef struct {
  i2c_t* i2c;
  i2cmux_version_t version;
  uint8_t address;
} i2cmux_t;

//TODO add interrupt masks

// Public
i2cmux_t* MuxI2cInit(i2c_t* i2c, uint8_t addrs, i2cmux_version_t ver);
uint8_t MuxI2cRead(const i2cmux_t* mux);
void MuxI2cSetChannel(const i2cmux_t* mux, uint8_t chan);
void MuxI2cFree(i2cmux_t* mux);


#endif /* _PCA9548_H_ */
