#include "i2cdriver.h"

void i2cInit(void)
{
  /* CM_PER_I2C1_CLKCTRL: MODULEMODE = 0x2 - enable */
  HWREG(0x44E00000 + 0x48) = 0x2;
  __delay_cycles(1000);

  /* Reset/disable i2c */
  I2CMasterDisable(SOC_I2C_1_REGS);

  /* Disable auotidle */
  I2CAutoIdleDisable(SOC_I2C_1_REGS);

  /* Bus speed 400 kHz */
  I2CMasterInitExpClk(SOC_I2C_1_REGS, 48000000, 12000000, 400000);

  /* Enable */
  I2CMasterEnable(SOC_I2C_1_REGS);

  __delay_cycles(5000);
}

void i2cCleanUp(void)
{
  /* I2C_CON : reset */
  HWREG(SOC_I2C_1_REGS + 0xA4) = 0x0;

  /* I2C_SYS : solft reset */
  HWREG(SOC_I2C_1_REGS + 0x10) = (1 << 1);
}

void i2cClearInterrupts(void)
{
  /* I2C_IRQSTATUS: Clear all */
  HWREG(SOC_I2C_1_REGS + 0x28) = 0x7FFF;

  /* I2C_IRQENABLE_CLR: Clear all */
  HWREG(SOC_I2C_1_REGS + 0x30) = 0x7FFF;
}

uint8_t i2cRxByte(uint8_t addr, uint8_t reg)
{
  uint8_t rtn = 0;

  i2cClearInterrupts();

  /* Slave address */
  I2CMasterSlaveAddrSet(SOC_I2C_1_REGS, addr);

  /* Tx 1 byte first - devReg */
  I2CSetDataCount(SOC_I2C_1_REGS, 0x1);

  /* Configure as master transmitter */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(SOC_I2C_1_REGS));

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, reg);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready int */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS);

  /* Rx 1 byte (response) */
  I2CSetDataCount(SOC_I2C_1_REGS, 0x1);

  /* Configure as master reciever */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_RX);

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  /* Wait for receive ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_RECV_READY)));

  /* Get data from fifo */
  rtn = I2CMasterDataGet(SOC_I2C_1_REGS);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_RECV_READY);

  /* Stop condition */
  I2CMasterStop(SOC_I2C_1_REGS);

  i2cClearInterrupts();

  return rtn;
}

void i2cRxBurst(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buffer)
{
  uint16_t i = 0;

  i2cClearInterrupts();

  /* Slave address */
  I2CMasterSlaveAddrSet(SOC_I2C_1_REGS, addr);

  /* Configure as master transmitter */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_TX);

  /* Tx 1 byte first - devReg */
  I2CSetDataCount(SOC_I2C_1_REGS, 0x1);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(SOC_I2C_1_REGS));

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, reg);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready int */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS);

  /* Configure as master reciever */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_RX);

  /* Rx bytes (num of data bytes) */
  I2CSetDataCount(SOC_I2C_1_REGS, len);

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  while(I2CDataCountGet(SOC_I2C_1_REGS) != 0){

    /* Wait for receive ready */
    while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_RECV_READY)));

    buffer[i++] = I2CMasterDataGet(SOC_I2C_1_REGS);

    /* Clear int */
    I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_RECV_READY);
  }

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(SOC_I2C_1_REGS);

  i2cClearInterrupts();
}

void i2cTxByte(uint8_t addr, uint8_t reg, uint8_t tx)
{
  i2cClearInterrupts();

  /* Slave address */
  I2CMasterSlaveAddrSet(SOC_I2C_1_REGS, addr);

  /* Tx: 2 bytes  devReg, Tx */
  I2CSetDataCount(SOC_I2C_1_REGS, 0x2);

  /* Configure as master transmitter */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(SOC_I2C_1_REGS));

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));


  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, reg);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));

  /* Write payload to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, tx);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(SOC_I2C_1_REGS);

  i2cClearInterrupts();
}

void i2cTxBurst(uint8_t addr, uint8_t reg, uint16_t len, uint8_t* tx)
{
  uint16_t i = 0;

  i2cClearInterrupts();

  /* Slave address */
  I2CMasterSlaveAddrSet(SOC_I2C_1_REGS, addr);

  /* Tx: 2 bytes  devReg, Tx */
  I2CSetDataCount(SOC_I2C_1_REGS, (0x1 + len));

  /* Configure as master transmitter */
  I2CMasterControl(SOC_I2C_1_REGS, I2C_CFG_MST_TX);

  /* Enable transmit, recieve and registers ready interrupt */
  I2CMasterIntEnableEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY
                                  | I2C_INT_RECV_READY
                                  | I2C_INT_ADRR_READY_ACESS);

  /* Make sure bus is free */
  while(I2CMasterBusBusy(SOC_I2C_1_REGS));

  /* Start condition */
  I2CMasterStart(SOC_I2C_1_REGS);

  /* Wait for transmit ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));

  /* Write devive register (devReg) to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, reg);

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);

  while(I2CDataCountGet(SOC_I2C_1_REGS) != 0){

    /* Wait for transmit ready */
    while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY)));

    /* Write payload to fifo */
    I2CMasterDataPut(SOC_I2C_1_REGS, tx[i++]);

    /* Clear int */
    I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_TRANSMIT_READY);
  }

  /* Wait for registers ready */
  while(!(I2CMasterIntStatusEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS)));

  /* Clear int */
  I2CMasterIntClearEx(SOC_I2C_1_REGS, I2C_INT_ADRR_READY_ACESS);

  /* Stop condition */
  I2CMasterStop(SOC_I2C_1_REGS);

  i2cClearInterrupts();
}

