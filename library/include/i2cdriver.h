/* Copyright 2017 Jonathan Realmuto

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

#ifndef _I2CDRIVER_H_
#define _I2CDRIVER_H_


typedef struct {
  uint32_t regmap;
} i2c_t;

/* ---- Prototypes ---- */
i2c_t* I2cInit(uint8_t channel);
void I2cCleanUp(i2c_t* i2c);
uint8_t I2cRxByte(const i2c_t* i2c, uint8_t addr, uint8_t reg);
void I2cTxByte(const i2c_t* i2c,uint8_t addr, uint8_t reg, uint8_t tx);
void I2cRxBurst(const i2c_t* i2c,
              uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buf);
void I2cRxBurstNoReg(const i2c_t* i2c,
              uint8_t addr, uint16_t len, uint8_t *buffer);
void I2cTxByteNoReg(const i2c_t* i2c, uint8_t addr, uint8_t tx);

#endif
