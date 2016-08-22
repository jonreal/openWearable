#ifndef _I2CDRIVER_H_
#define _I2CDRIVER_H_

#include <stdint.h>
#include "soc_AM33XX.h"
#include "hw_types.h"
#include "hw_hsi2c.h"
#include "hsi2c.h"

extern volatile uint32_t* debugBuffer;

/* ---- Prototypes ---- */
void i2cInit(void);
void i2cCleanUp(void);
uint8_t i2cRxByte(uint8_t addr, uint8_t reg);
void i2cTxByte(uint8_t addr, uint8_t reg, uint8_t tx);
void i2cRxBurst(uint8_t addr, uint8_t reg, uint16_t len, uint8_t *buf);
void i2cTxBurst(uint8_t addr, uint8_t reg, uint16_t len, uint8_t* tx);
void i2cClearInterrupts(void);

#endif
