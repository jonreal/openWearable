/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * Minimal J721E EHRPWM memmap driver (custom PWM), core-agnostic. Used from the
 * R5F to generate the ADS131M08 CLKIN in firmware instead of the Linux sysfs PWM.
 *
 * Same 16-bit EHRPWM register IP the TI pwm-tiehrpwm driver uses. Instance bases
 * are 256-byte blocks: EHRPWM0 = 0x03000000 ... EHRPWM2 = 0x03020000 ...  The
 * time-base clock (TBCLK) source is 125 MHz.
 *
 * CLOCK NOTE: like McSPI7, the R5F cannot enable the J721E functional clock --
 * Linux must keep the EHRPWM clock on (pin 3020000.pwm runtime-PM). This driver
 * only programs the registers, which the R5F reaches by memmap.
 */

#ifndef OW_EPWM_J721E_H_
#define OW_EPWM_J721E_H_

#include <stdint.h>

#define EPWM2_BASE     0x03020000u
#define EPWM_TBCLK_HZ  125000000u

/* Free-running ~50%-duty square wave at freq_hz on output A (up-count mode). */
void     epwmInitClock(uint32_t base, uint32_t freq_hz);

/* Read TBPRD back -- reach + clock self-check (nonzero after init = reachable). */
uint16_t epwmReach(uint32_t base);

/* Freeze the time-base counter (stops the output). */
void     epwmStop(uint32_t base);

#endif /* OW_EPWM_J721E_H_ */
