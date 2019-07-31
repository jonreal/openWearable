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

#include <stdlib.h>
#include <stdint.h>

#include "soc_AM33XX.h"
#include "hw_types.h"
#include "hw_hsi2c.h"
#include "hsi2c.h"
#include "i2cdriver.h"

extern volatile uint32_t* debug_buff;


static void I2cClearInterrupts(uint32_t regmap)
{
  /* I2C_IRQSTATUS: Clear all */
  HWREG(regmap + 0x28) = 0x7FFF;

  /* I2C_IRQENABLE_CLR: Clear all */
  HWREG(regmap + 0x30) = 0x7FFF;
}

i2c_t* I2cInit(uint8_t channel)
{
  i2c_t* i2c = malloc(sizeof(i2c_t));
  switch (channel) {
    case 1 :
      i2c->regmap = SOC_I2C_1_REGS;
      /* CM_PER_I2C1_CLKCTRL: MODULEMODE = 0x2 - enable */
      HWREG(0x44E00000 + 0x48) = 0x2;
#ifdef PRU
      __delay_cycles(1000);
#else
      usleep(1000);
#endif
      break;
    case 2 :
      i2c->regmap = SOC_I2C_2_REGS;
      /* CM_PER_I2C2_CLKCTRL: MODULEMODE = 0x2 - enable */
      HWREG(0x44E00000 + 0x44) = 0x2;
#ifdef PRU
      __delay_cycles(1000);
#else
      usleep(1000);
#endif
      break;
  }

  /* I2C_SYS : soft reset */
  HWREG(i2c->regmap + 0x10) = (1 << 1);

#ifdef PRU
  __delay_cycles(1000);
#else
  usleep(1000);
#endif

  /* Reset/disable i2c */
  I2CMasterDisable(i2c->regmap);

  /* Disable auotidle */
  I2CAutoIdleDisable(i2c->regmap);

  /* Bus speed 400 kHz */
  I2CMasterInitExpClk(i2c->regmap, 48000000, 12000000, 400000);

#ifdef PRU
  __delay_cycles(1000);
#else
  usleep(1000);
#endif

  /* Enable */
  I2CMasterEnable(i2c->regmap);

#ifdef PRU
  __delay_cycles(5000);
#else
  usleep(5000);
#endif

  return i2c;
}

void I2cCleanUp(i2c_t* i2c)
{
  /* I2C_CON : reset */
  HWREG(i2c->regmap + 0xA4) = 0x0;

  /* I2C_SYS : solft reset */
  HWREG(i2c->regmap + 0x10) = (1 << 1);

  free(i2c);
}

uint8_t I2cRxByte(const i2c_t* i2c, uint8_t addr, uint8_t reg)
{
  uint8_t rtn = 0;

  I2cClearInterrupts(i2c->regmap);

  /* Slave address */
  I2CMasterSlaveAddrSet(i2c->regmap, addr);

  /* Tx 1 byte first - devReg */
  I2CSetDataCount(i2c->regmap, 0x1);

  /* Configure as master transmitter */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(i2c->regmap, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(i2c->regmap));

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(i2c->regmap, reg);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready int */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Rx 1 byte (response) */
  I2CSetDataCount(i2c->regmap, 0x1);

  /* Configure as master reciever */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_RX);

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  /* Wait for receive ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_RECV_READY)));

  /* Get data from fifo */
  rtn = I2CMasterDataGet(i2c->regmap);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_RECV_READY);

  /* Stop condition */
  I2CMasterStop(i2c->regmap);

  I2cClearInterrupts(i2c->regmap);

  return rtn;
}

void I2cRxBurst(const i2c_t* i2c,
                  uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buffer)
{
  uint16_t i = 0;

  I2cClearInterrupts(i2c->regmap);

  /* Slave address */
  I2CMasterSlaveAddrSet(i2c->regmap, addr);

  /* Configure as master transmitter */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_TX);

  /* Tx 1 byte first - devReg */
  I2CSetDataCount(i2c->regmap, 0x1);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(i2c->regmap, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(i2c->regmap));

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(i2c->regmap, reg);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready int */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Configure as master reciever */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_RX);

  /* Rx bytes (num of data bytes) */
  I2CSetDataCount(i2c->regmap, len);

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  while(I2CDataCountGet(i2c->regmap) != 0){

    /* Wait for receive ready */
    while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_RECV_READY)));

    buffer[i++] = I2CMasterDataGet(i2c->regmap);

    /* Clear int */
    I2CMasterIntClearEx(i2c->regmap, I2C_INT_RECV_READY);
  }

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(i2c->regmap);

  I2cClearInterrupts(i2c->regmap);
}



void I2cTxByte(const i2c_t* i2c, uint8_t addr, uint8_t reg, uint8_t tx)
{
  I2cClearInterrupts(i2c->regmap);

  /* Slave address */
  I2CMasterSlaveAddrSet(i2c->regmap, addr);

  /* Tx: 2 bytes  devReg, Tx */
  I2CSetDataCount(i2c->regmap, 0x2);

  /* Configure as master transmitter */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(i2c->regmap, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(i2c->regmap));

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(i2c->regmap, reg);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_TRANSMIT_READY);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_TRANSMIT_READY)));

  /* Write payload to fifo */
  I2CMasterDataPut(i2c->regmap, tx);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(i2c->regmap);

  I2cClearInterrupts(i2c->regmap);
}

void I2cTxByteNoReg(const i2c_t* i2c, uint8_t addr, uint8_t tx)
{
  I2cClearInterrupts(i2c->regmap);

  /* Slave address */
  I2CMasterSlaveAddrSet(i2c->regmap, addr);

  /* Tx: 1 bytes tx */
  I2CSetDataCount(i2c->regmap, 0x1);

  /* Configure as master transmitter */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(i2c->regmap, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(i2c->regmap));

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_TRANSMIT_READY)));

  /* Write payload (tx) to fifo */
  I2CMasterDataPut(i2c->regmap, tx);

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(i2c->regmap);

  I2cClearInterrupts(i2c->regmap);
}

void I2cRxBurstNoReg(const i2c_t* i2c,
                uint8_t addr, uint16_t len, uint8_t *buffer)
{
  uint16_t i = 0;

  I2cClearInterrupts(i2c->regmap);

  /* Slave address */
  I2CMasterSlaveAddrSet(i2c->regmap, addr);

  /* Configure as master reciever */
  I2CMasterControl(i2c->regmap, I2C_CFG_MST_RX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(i2c->regmap, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Rx bytes (num of data bytes) */
  I2CSetDataCount(i2c->regmap, len);

  /* Start condition */
  I2CMasterStart(i2c->regmap);

  while(I2CDataCountGet(i2c->regmap) != 0){

    /* Wait for receive ready */
    while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_RECV_READY)));

    buffer[i++] = I2CMasterDataGet(i2c->regmap);

    /* Clear int */
    I2CMasterIntClearEx(i2c->regmap, I2C_INT_RECV_READY);
  }

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(i2c->regmap, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(i2c->regmap);

  I2cClearInterrupts(i2c->regmap);
}
