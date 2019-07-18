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

#include "pca9548a.h"
#include <stdlib.h>
#include "i2cdriver.h"

// Driver works with pca9548a and pca9544a
//
// pca9548 - 8 channels
// pca9544 - 4 channels

i2cmux_t* MuxI2cInit(uint8_t addrs, i2cmux_version_t ver) {
  i2cmux_t* mux = malloc(sizeof(i2cmux_t));
  mux->address = addrs;
  mux->version = ver;
  return mux;
}

uint8_t MuxI2cRead(const i2cmux_t* mux) {
  uint8_t rx = 0;

  return rx;
}

void MuxI2cSetChannel(const i2cmux_t* mux, uint8_t chan){

  if (mux->version == PCA9548)
    I2cTxByteNoReg(mux->address, 1 << chan);
  else
    I2cTxByteNoReg(mux->address, (chan + 4));
}

void MuxI2cFree(i2cmux_t* mux) {
  free(mux);
}
