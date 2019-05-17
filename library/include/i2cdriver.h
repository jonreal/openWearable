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

extern volatile uint32_t* debug_buff;

/* ---- Prototypes ---- */
void I2cInit(void);
void I2cCleanUp(void);
uint8_t I2cRxByte(uint8_t addr, uint8_t reg);
void I2cTxByte(uint8_t addr, uint8_t reg, uint8_t tx);
void I2cRxBurst(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buf);
void I2cRxBurstNoReg(uint8_t addr, uint16_t len, uint8_t *buffer);
void I2cTxByteNoReg(uint8_t addr, uint8_t tx);
//uint8_t I2cRxByteNoReg(uint8_t addr);
void I2cClearInterrupts(void);

#endif
