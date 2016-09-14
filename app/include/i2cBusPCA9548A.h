#ifndef _I2CBUSPCA9548A_H_
#define _I2CBUSPCA9548A_H_

#include <stdint.h>
#include "soc_AM33XX.h"
#include "hw_types.h"
#include "hw_hsi2c.h"
#include "hsi2c.h"

#include "i2cdriver.h"

#define I2C_BUS_ADDR 0x70

extern volatile uint32_t* debugBuffer;

void i2cBusChannelOn(uint8_t ch_id);
void i2cBusCleanUp(void);

#endif
