#include "i2cBusPCA9548A.h"


// valid channels 1-8
void i2cBusChannelOn(uint8_t channel)
{

  i2cClearInterrupts();

  /* Slave address */
  I2CMasterSlaveAddrSet(SOC_I2C_1_REGS, I2C_BUS_ADDR);

  /* Tx: 1 byte channel */
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

  /* Write ch_id to fifo */
  I2CMasterDataPut(SOC_I2C_1_REGS, (1 << (channel-1)));

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


void i2cBusCleanUp(void)
{
  i2cBusChannelOn(0x00);
}
