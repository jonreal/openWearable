/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * J721E EHRPWM memmap driver -- see epwm_j721e.h. Port of the archive AM335x
 * pwmdriver.c EPWM config (same 16-bit IP), base passed in, no PWMSS clock poke
 * (Linux provisions the functional clock on J721E). Pure memmap C.
 */

#include "epwm_j721e.h"

#define HWREGH(a) (*((volatile uint16_t *)(uintptr_t)(a)))

/* 16-bit EHRPWM register offsets (identical AM335x <-> J721E). */
#define EPWM_TBCTL   0x00u   /* CTRMODE[1:0]: 0=up, 3=freeze; CLKDIV=0 -> TBCLK=fck */
#define EPWM_TBCNT   0x08u
#define EPWM_TBPRD   0x0Au
#define EPWM_CMPCTL  0x0Eu
#define EPWM_CMPA    0x12u
#define EPWM_AQCTLA  0x16u   /* ZRO[1:0], PRD[3:2], CAU[5:4], ... (0=x,1=low,2=high,3=tgl) */

void epwmInitClock(uint32_t base, uint32_t freq_hz)
{
  /* Up-count: F = TBCLK/(TBPRD+1). CMPA = (TBPRD+1)/2 -> ~50% duty. */
  uint16_t prd  = (uint16_t)((EPWM_TBCLK_HZ / freq_hz) - 1u);
  uint16_t cmpa = (uint16_t)((prd + 1u) / 2u);

  HWREGH(base + EPWM_TBCTL)  = 0x0003u;                 /* freeze while configuring */
  HWREGH(base + EPWM_TBCNT)  = 0x0000u;
  HWREGH(base + EPWM_TBPRD)  = prd;
  HWREGH(base + EPWM_CMPCTL) = 0x0000u;                 /* shadow, load CMPA at CTR=0 */
  HWREGH(base + EPWM_CMPA)   = cmpa;
  HWREGH(base + EPWM_AQCTLA) = (2u << 0) | (1u << 4);   /* ZRO=set-high, CAU=clear-low */
  HWREGH(base + EPWM_TBCTL)  = 0x0000u;                 /* up-count, TBCLK=fck -> run */
}

uint16_t epwmReach(uint32_t base)
{
  return HWREGH(base + EPWM_TBPRD);
}

void epwmStop(uint32_t base)
{
  HWREGH(base + EPWM_TBCTL) = 0x0003u;                  /* freeze counter */
}
