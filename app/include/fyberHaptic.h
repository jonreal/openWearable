#ifndef _FYBER_HAPTIC_H_
#define _FYBER_HAPTIC_H_

#include "i2cdriver.h"
#include "i2cBusPCA9548A.h"

#define HAPTIC_I2C_ADDR      0x59

extern volatile uint32_t* debugBuffer;

void hapticInit(void);
void hapticCleanUp(void);
void hapticSetWaveForm(void);
void setAmplitude(uint8_t ch, uint32_t cnt, int8_t amp);

#endif
