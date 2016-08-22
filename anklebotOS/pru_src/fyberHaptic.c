#include <stdint.h>

#include "fyberHaptic.h"

void hapticInit(void)
{
  uint8_t rx;

  // Check chip ID
  rx = i2cRxByte(HAPTIC_I2C_ADDR, 0x01);
  __delay_cycles(10000);

  // Take out of standby mode
  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x00);
  __delay_cycles(10000);

  // 100 V gain
  i2cTxByte(HAPTIC_I2C_ADDR, 0x01, 0x03);
  __delay_cycles(10000);
}

void setAmplitude(uint32_t cnt, int8_t amp)
{
  if (amp != 0) {
    if ((cnt % 2) == 0)
      i2cTxByte(HAPTIC_I2C_ADDR, 0x0B, amp);
    else
      i2cTxByte(HAPTIC_I2C_ADDR, 0x0B, -amp);
  }
}

void hapticSetWaveForm(void)
{
  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x01, 0x03);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x03, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x04, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0xFF, 0x01);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x00, 0x05);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x01, 0x80);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x06);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x03, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x04, 0x09);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x05, 0xFF);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x06, 0xFF);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x07, 0x19);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x08, 0x05);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x09, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0xFF, 0x00);
  __delay_cycles(10000);

  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x01);
  __delay_cycles(10000);

}

void hapticCleanUp(void)
{
  // Put device in standby mode
  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x40);
}
