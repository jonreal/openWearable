#include <stdint.h>

#include "fyberHaptic.h"

void hapticInit(void)
{
  for (int i=2; i<7; i++)
  {
    // Mux I2C bus
    i2cBusChannelOn(i);
    __delay_cycles(5000);

    // Take out of standby mode
    i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x00);
    __delay_cycles(5000);

    // 100 V gain
    i2cTxByte(HAPTIC_I2C_ADDR, 0x01, 0x01);
    __delay_cycles(5000);
  }
}

void setAmplitude(uint8_t ch, uint32_t cnt, int8_t amp)
{
  i2cBusChannelOn(ch);
  __delay_cycles(5000);

  if (amp != 0) {
    if ((cnt % 2) == 0)
      i2cTxByte(HAPTIC_I2C_ADDR, 0x0B, amp);
    else
      i2cTxByte(HAPTIC_I2C_ADDR, 0x0B, -amp);
  }
  else{
    i2cTxByte(HAPTIC_I2C_ADDR, 0x0B, 0);
  }
  __delay_cycles(5000);
}


void hapticCleanUp(void)
{
  // Put device in standby mode
  i2cTxByte(HAPTIC_I2C_ADDR, 0x02, 0x40);
}



