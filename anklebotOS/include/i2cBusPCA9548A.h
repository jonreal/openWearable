#ifndef _I2CBUSPCA9548A_H_
#define _I2CBUSPCA9548A_H_

#include <stdint.h>
#include "soc_AM33XX.h"
#include "hw_types.h"
#include "hw_hsi2c.h"
#include "hsi2c.h"

#include "i2cdriver.h"

extern volatile uint32_t* debugBuffer;

#define I2C_BUS_ADDR      0x70
#define I2C_BUS_CH0_EN    0x01
#define I2C_BUS_CH1_EN    0x02
#define I2C_BUS_CH2_EN    0x04
#define I2C_BUS_CH3_EN    0x08
#define I2C_BUS_CH4_EN    0x10
#define I2C_BUS_CH5_EN    0x20


void i2cBusChannelOn(uint8_t ch_id);
void i2cBusCleanUp(void);

#endif
