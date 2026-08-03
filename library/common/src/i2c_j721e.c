/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * J721E OMAP-I2C driver -- port of library/pru/src/i2cdriver.c (AM335x). Same IP,
 * base passed in, no CM_PER, no malloc. This file provides bring-up (reset +
 * master config) and the reach self-check; bus transactions (the StarterWare
 * Rx/Tx layer) are deferred until a device is wired to the bus. */

#include <stdint.h>
#include "i2c_j721e.h"

#define HWREG(a) (*((volatile uint32_t *)(uintptr_t)(a)))

/* OMAP I2C register offsets (identical AM335x <-> J721E). */
#define I2C_REVNB_LO    0x000u
#define I2C_SYSC        0x010u   /* SRST = bit 1                              */
#define I2C_CON         0x0A4u   /* I2C_EN = bit 15, MST = bit 10             */
#define I2C_PSC         0x0B0u   /* prescaler (func clk -> internal clk)      */
#define I2C_SCLL        0x0B4u
#define I2C_SCLH        0x0B8u

#define CON_I2C_EN      (1u << 15)
#define CON_MST         (1u << 10)
#define SYSC_SRST       (1u << 1)

static void i2c_delay(void)
{
  for (volatile uint32_t i = 0u; i < 20000u; ++i) { }
}

uint32_t i2cRev(uint32_t base)
{
  return HWREG(base + I2C_REVNB_LO);
}

void i2cInit(uint32_t base)
{
  /* No CM_PER write (AM335x had one) -- clock is provisioned outside the R5F. */

  HWREG(base + I2C_CON)  = 0u;            /* disable while configuring */
  HWREG(base + I2C_SYSC) = SYSC_SRST;     /* soft reset */
  i2c_delay();

  /* 96 MHz func clk -> 12 MHz internal (PSC=7); 12 MHz -> 400 kHz (div=30):
   * SCLL = div/2 - 7 = 8, SCLH = div/2 - 5 = 10. VERIFY the func clock. */
  HWREG(base + I2C_CON)  = 0u;
  HWREG(base + I2C_PSC)  = 7u;
  HWREG(base + I2C_SCLL) = 8u;
  HWREG(base + I2C_SCLH) = 10u;

  HWREG(base + I2C_CON)  = CON_I2C_EN | CON_MST;   /* enable, master mode */
  i2c_delay();
}
