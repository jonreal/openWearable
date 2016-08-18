#ifndef _FYBER_HAPTIC_H_
#define _FYBER_HAPTIC_H_

#include "i2cdriver.h"

#define HAPITIC_I2C_ADDR      0x59

extern volatile uint32_t* debugBuffer;

void hapticInit(void);
void hapticCleanUp(void);
void hapticSetWaveForm(void);


#endif
