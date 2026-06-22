/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * J721E OMAP-I2C driver for the R5F (and reusable elsewhere). Port of the AM335x
 * library/pru/src/i2cdriver.c with the same two deltas as the McSPI port: the
 * controller BASE is passed in (no SOC_I2C_x_REGS / malloc), and the AM335x
 * CM_PER clock-enable is gone (J721E clocks via Linux/TISCI). The OMAP I2C IP is
 * the same register map AM335x <-> J721E. See docs/COMM-BUS-LAYOUT.md.
 *
 * Main-domain I2C bases (J721E): I2C(n) = 0x2000000 + n*0x10000.
 */

#ifndef OW_I2C_J721E_H_
#define OW_I2C_J721E_H_

#include <stdint.h>

#define I2C2_BASE   0x02020000u   /* R5F bus (P9.19/P9.20) */

/* Read I2C_REVNB_LO (base + 0x000). A sane non-zero IP id is the FIRST access a
 * core makes -- it proves the master reached the register block (clock on +
 * firewall open) before any bus transaction. */
uint32_t i2cRev(uint32_t base);

/* Soft-reset + master-mode bring-up (no bus traffic, no polling -> cannot hang).
 * Prescaler/SCLL/SCLH assume a 96 MHz functional clock for ~400 kHz; VERIFY the
 * J721E main_i2c functional clock before relying on the bus speed. */
void i2cInit(uint32_t base);

#endif /* OW_I2C_J721E_H_ */
