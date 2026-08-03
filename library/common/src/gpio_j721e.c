/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * J721E main-GPIO memmap driver (ti,keystone-gpio) -- see gpio_j721e.h.
 * Pure memmap C; here linked into the R5F firmware to drive a manual chip-select.
 */

#include "gpio_j721e.h"

#define HWREG(a) (*((volatile uint32_t *)(uintptr_t)(a)))

/* Bank-0 register offsets from the instance base (lines 0..31). */
#define GPIO_DIR  0x10u   /* 1 = input, 0 = output */
#define GPIO_OUT  0x14u
#define GPIO_SET  0x18u   /* write 1 -> drive line high (per-bit, atomic) */
#define GPIO_CLR  0x1Cu   /* write 1 -> drive line low  (per-bit, atomic) */
#define GPIO_IN   0x20u

uint32_t gpioReach(uint32_t base)
{
  return HWREG(base + GPIO_DIR);
}

void gpioSet(uint32_t base, uint32_t bit)
{
  HWREG(base + GPIO_SET) = bit;
}

void gpioClr(uint32_t base, uint32_t bit)
{
  HWREG(base + GPIO_CLR) = bit;
}

void gpioInitOutput(uint32_t base, uint32_t bit)
{
  HWREG(base + GPIO_SET)  = bit;    /* preset high (idle) before enabling output */
  HWREG(base + GPIO_DIR) &= ~bit;   /* 0 = output */
}
