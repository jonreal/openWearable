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

/* J721E (TDA4VM) McSPI driver for the PRU/R5F.
 *
 * The OMAP McSPI IP is IDENTICAL on AM335x and J721E (Linux: ti,omap4-mcspi,
 * regs_offset 0x100), so this is the AM335x `spidriver.c` with two changes:
 *   1. the controller BASE is passed in (no hard-coded SOC_SPI_1_REGS), so the
 *      same code drives McSPI6 (0x2160000) and McSPI7 (0x2170000);
 *   2. the AM335x CM_PER clock-enable write is GONE — on J721E the functional
 *      clock is provisioned outside the firmware (Linux/TISCI). See
 *      docs/COMM-BUS-LAYOUT.md.
 *
 * Main-domain McSPI bases (J721E): McSPI(n) = 0x2100000 + n*0x10000.
 */

#ifndef OW_MCSPI_J721E_H_
#define OW_MCSPI_J721E_H_

#include <stdint.h>

#define MCSPI6_BASE   0x02160000u   /* pru0_1 bus */
#define MCSPI7_BASE   0x02170000u   /* pru0_0 bus */

/* Read MCSPI_HL_REV (base + 0x000). Reset value is a fixed, non-zero IP id.
 * This is the FIRST access a core makes to the controller: a sane readback
 * proves the master can reach the register block (clock on + firewall open)
 * BEFORE any transfer is attempted. */
uint32_t mcspiHlRev(uint32_t base);

/* Soft-reset + master-mode init of channel 0 (full-duplex, EPOL active-low CS).
 * wl_bits = word length in bits (1..32, e.g. 8 or 16); clkdiv selects the SPI
 * clock from the 48 MHz functional clock via CLKG=1 + EXTCLK. */
void mcspiInit(uint32_t base, uint8_t wl_bits, uint16_t clkdiv);

/* One channel-0 transfer: write tx, poll EOT, return the received word. */
uint32_t mcspiXfer(uint32_t base, uint32_t tx);

/* Disable channel 0 (leaves the clock alone — owned by Linux/TISCI on J721E). */
void mcspiCleanup(uint32_t base);

#endif /* OW_MCSPI_J721E_H_ */
