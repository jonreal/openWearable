/* test-ads131m08 R5F hooks -- main_r5fss0 drives the ADS131M08 via the ads131
 * device driver: SPI on McSPI7 + GPIO chip-select on P9.28, all in firmware.
 * R5f0Init sets it up + reach self-check; R5f0UpdateState reads all 8 channels
 * each (decimated) tick into r5f_state, which PRU0 snapshots into the log ring.
 *
 * CLKIN is NOT generated here: the J721E EHRPWM TBCLK is a Linux-owned clock the
 * R5F cannot enable, so the A72 generates the ~7.8 MHz CLKIN on EHRPWM2_A (P9.14)
 * with the pwm framework in PruLoadParams (uiloop.c). See README.md.
=============================================================================*/
#include "r5floop.h"
#include "mcspi_j721e.h"
#include "gpio_j721e.h"
#include "ads131.h"

static const ads131_cfg_t adc_cfg = {
  .spi_base = MCSPI7_BASE,     // pru0_0 bus = McSPI7 (P9.29/30/31)
  .sclk_div = 200u,            // SCLK divider (conservative)
  .cs_base  = GPIO1_BASE,      // CS = P9.28 = main_gpio1 line 11
  .cs_bit   = (1u << 11),
  .cs_pad   = 0x0011C230u,     // P9.28 CTRL_MMR pad -> GPIO (self-contained remux)
  .pwm_base = 0u,              // CLKIN via Linux pwm (uiloop.c); R5F can't gate TBCLK
  .clkin_hz = 0u,              // (unused when pwm_base == 0)
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
