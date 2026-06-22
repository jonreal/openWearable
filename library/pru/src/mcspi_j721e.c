/* Copyright 2026 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

/* J721E McSPI driver — port of library/pru/src/spidriver.c (AM335x).
 * Same OMAP McSPI IP; the only deltas are (1) base passed in, (2) no CM_PER.
 * Register block: regs_offset 0x100 (ti,omap4-mcspi), so the functional
 * registers (SYSCONFIG, MODULCTRL, CH0CONF, ...) sit at base + 0x11x/0x12x/0x13x.
 * MCSPI_HL_REV is at base + 0x000. */

#include <stdint.h>
#include "mcspi_j721e.h"

#define HWREG(a) (*((volatile uint32_t *)(uintptr_t)(a)))

/* Functional-register offsets (regs_offset 0x100). Identical AM335x <-> J721E. */
#define MCSPI_HL_REV    0x000u
#define MCSPI_SYSCONFIG 0x110u
#define MCSPI_SYSSTATUS 0x114u
#define MCSPI_IRQSTATUS 0x118u
#define MCSPI_IRQENABLE 0x11Cu
#define MCSPI_MODULCTRL 0x128u
#define MCSPI_CH0CONF   0x12Cu
#define MCSPI_CH0STAT   0x130u
#define MCSPI_CH0CTRL   0x134u
#define MCSPI_TX0       0x138u
#define MCSPI_RX0       0x13Cu

/* MCSPI_CHxCONF bit fields (standard OMAP McSPI) */
#define CHCONF_PHA      (1u << 0)    /* clock phase                              */
#define CHCONF_POL      (1u << 1)    /* clock polarity                           */
#define CHCONF_CLKD(d)  (((d) & 0xFu) << 2)
#define CHCONF_EPOL     (1u << 6)    /* 1 = SPIEN (CS) active low                */
#define CHCONF_WL(b)    ((((b) - 1u) & 0x1Fu) << 7) /* word length, b bits       */
#define CHCONF_DPE0     (1u << 16)   /* 1 = no transmit on D0                    */
#define CHCONF_DPE1     (1u << 17)   /* 0 = transmit on D1 (MOSI)                */
#define CHCONF_IS       (1u << 18)   /* 0 (default) = receive on D0 (MISO)       */
#define CHCONF_CLKG     (1u << 29)   /* 1-cycle clock granularity (uses EXTCLK)  */

#define CHSTAT_RXS      (1u << 0)    /* RX register full (clears when RX0 read)   */
#define CHSTAT_TXS      (1u << 1)    /* TX register empty                         */
#define CHSTAT_EOT      (1u << 2)    /* end of transfer                           */
#define CHCTRL_EN       (1u << 0)

/* RX0 holds the running 32-bit shift register; the current word is its low
 * wl_bits. Mask to wl_bits on read (the AM335x driver did this via a uint8_t
 * cast). Set by mcspiInit. */
static uint32_t s_rxmask = 0xFFFFFFFFu;

uint32_t mcspiHlRev(uint32_t base)
{
  /* First touch of the controller: a sane (non-zero) IP revision proves the
   * core reached the register block (clock on + firewall open). */
  return HWREG(base + MCSPI_HL_REV);
}

void mcspiInit(uint32_t base, uint8_t wl_bits, uint16_t clkdiv)
{
  /* NOTE: no CM_PER write here (AM335x had one) — the McSPI functional clock
   * is provisioned outside the firmware on J721E (Linux/TISCI). */

  /* Soft reset, then wait for SYSSTATUS.RESETDONE. */
  HWREG(base + MCSPI_SYSCONFIG) = (1u << 1);
  while ((HWREG(base + MCSPI_SYSSTATUS) & 0x1u) != 0x1u) { }

  /* Master mode, multi-channel, SPIEN used as CS, functional mode. */
  HWREG(base + MCSPI_MODULCTRL) = 0x00000000u;

  /* CLOCKACTIVITY = 3 (OCP + func clocks maintained), SIDLEMODE = no-idle. */
  HWREG(base + MCSPI_SYSCONFIG) = (0x3u << 8) | (0x1u << 3);

  HWREG(base + MCSPI_IRQSTATUS) = 0xFFFFFFFFu;
  HWREG(base + MCSPI_IRQENABLE) = 0x00000000u;

  /* Channel 0: full-duplex master, TX on D1 (MOSI), RX on D0 (MISO),
   * CS active low, SPI mode 0 (POL=0, PHA=0), word length wl_bits.
   * IS=0 (RX on D0) + DPE0=1 (no TX on D0) + DPE1=0 (TX on D1) — matches Linux
   * omap2_mcspi and the AM335x spidriver.c. */
  HWREG(base + MCSPI_CH0CONF) = CHCONF_CLKG | CHCONF_DPE0
                              | CHCONF_WL(wl_bits) | CHCONF_EPOL
                              | CHCONF_CLKD(clkdiv);

  /* EXTCLK = upper bits of the clock divider; enable the channel. */
  HWREG(base + MCSPI_CH0CTRL) = (((uint32_t)(clkdiv >> 4) & 0xFFu) << 8) | CHCTRL_EN;

  s_rxmask = (wl_bits >= 32u) ? 0xFFFFFFFFu : ((1u << wl_bits) - 1u);
}

uint32_t mcspiXfer(uint32_t base, uint32_t tx)
{
  /* Poll TXS before writing and RXS before reading so each word is serialized.
   * EOT alone does not clear between back-to-back words -> stale/merged RX. */
  while ((HWREG(base + MCSPI_CH0STAT) & CHSTAT_TXS) == 0u) { }
  HWREG(base + MCSPI_TX0) = tx;
  while ((HWREG(base + MCSPI_CH0STAT) & CHSTAT_RXS) == 0u) { }
  return HWREG(base + MCSPI_RX0) & s_rxmask;
}

void mcspiCleanup(uint32_t base)
{
  HWREG(base + MCSPI_CH0CTRL) &= ~CHCTRL_EN;   /* leave the clock to Linux/TISCI */
}
