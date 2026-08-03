/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * Minimal J721E main-GPIO memmap driver (ti,keystone-gpio), core-agnostic.
 * Used from the R5F to bit-bang a chip-select the McSPI FORCE bit can't hold.
 *
 * Instance bases: main_gpio0 = 0x00600000 (128 lines), main_gpio1 = 0x00601000
 * (36 lines). Bank-0 (lines 0..31) register map -- VALIDATED on-board via devmem
 * (in_data tracked the drive):
 *   dir +0x10 (1=input, 0=output), out +0x14, set +0x18, clr +0x1C, in +0x20.
 * set/clr are write-1-to-set/clear (atomic per line -- no read-modify-write).
 * `bit` = (1u << line). Only bank 0 is exercised here (CS = main_gpio1 line 11).
 *
 * The pad still needs muxing to GPIO mode (CTRL_MMR) by the Linux-side bring-up;
 * this driver only drives the GPIO registers, which the R5F reaches by memmap.
 */

#ifndef OW_GPIO_J721E_H_
#define OW_GPIO_J721E_H_

#include <stdint.h>

#define GPIO0_BASE  0x00600000u
#define GPIO1_BASE  0x00601000u

uint32_t gpioReach(uint32_t base);                 /* read dir: reach self-check */
void     gpioInitOutput(uint32_t base, uint32_t bit); /* idle high, then dir=out */
void     gpioSet(uint32_t base, uint32_t bit);
void     gpioClr(uint32_t base, uint32_t bit);

#endif /* OW_GPIO_J721E_H_ */
