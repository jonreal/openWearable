/* test-ads131m08 R5F hooks -- main_r5fss0 drives the ADS131M08 via the ads131
 * device driver: SPI on McSPI7, GPIO chip-select on P9.28, and CLKIN generated
 * on EHRPWM2 (P9.14) -- all in firmware. R5f0Init sets it up + reach self-check;
 * R5f0UpdateState reads all 8 channels each (decimated) tick into r5f_state,
 * which PRU0 snapshots into the log ring.
 *
 * Linux-side prereq (R5F can't enable J721E clocks): pin EHRPWM2 + McSPI7
 * runtime-PM on before running -- see apps/test-ads131m08/adc-setup.sh.
=============================================================================*/
#include "r5floop.h"
#include "mcspi_j721e.h"
#include "gpio_j721e.h"
#include "epwm_j721e.h"
#include "ads131.h"

static const ads131_cfg_t adc_cfg = {
  .spi_base = MCSPI7_BASE,     // pru0_0 bus = McSPI7 (P9.29/30/31)
  .sclk_div = 200u,            // SCLK divider (conservative)
  .cs_base  = GPIO1_BASE,      // CS = P9.28 = main_gpio1 line 11
  .cs_bit   = (1u << 11),
  .cs_pad   = 0x0011C230u,     // P9.28 CTRL_MMR pad -> GPIO (self-contained remux)
  .pwm_base = EPWM2_BASE,      // CLKIN = EHRPWM2_A on P9.14
  .clkin_hz = 8192000u,        // ~8.192 MHz (TBPRD from 125 MHz TBCLK)
};

void R5f0Init(pru_mem_t* mem) {
  mem->s->r5f_state.hlrev = mcspiHlRev(MCSPI7_BASE);   // McSPI7 reach self-check
  mem->s->r5f_state.response = Ads131Init(&adc_cfg);   // ID (0x28xx expected)
}

void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io) {
  int32_t ch[8];
  Ads131ReadAllChannels(ch);
  for (int i = 0; i < 8; i++)
    io->s->adc[i] = ch[i];
  io->s->r5fvar++;
}

void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io) {}

void R5f0Cleanup(void) {}
