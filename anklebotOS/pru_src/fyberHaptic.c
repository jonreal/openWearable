#include <stdint.h>

#include "fyberHaptic.h"

void hapticInit(void)
{
  uint8_t rx;

  // Check chip ID
  rx = i2cRxByte(HAPITIC_I2C_ADDR, 0x01);

  __delay_cycles(10000);

  // Take out of standby mode
  i2cTxByte(HAPITIC_I2C_ADDR, 0x02, 0x00);
}

void hapticSetWaveForm(void)
{

  i2cTxByte(HAPITIC_I2C_ADDR, 0x01, 0x03);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x03, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x04, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0xFF, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x00, 0x05);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x01, 0x80);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x02, 0x06);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x03, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x04, 0x06);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x05, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x06+4, 0xFF);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x06+4+1, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x06+4+2, 0x10);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x06+4+3, 0x09);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0xFF, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPITIC_I2C_ADDR, 0x02, 0x01);
  __delay_cycles(10000);


  uint8_t rx = i2cRxByte(HAPITIC_I2C_ADDR, 0x01);

  debugBuffer[1] = rx;

}



void hapticCleanUp(void)
{
  // Put device in standby mode
  i2cTxByte(HAPITIC_I2C_ADDR, 0x02, 0x40);
}
