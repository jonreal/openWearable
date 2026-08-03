/* test-ads131m08 R5F hooks -- main_r5fss0 (lockstep) drives the ADS131M08 over
 * McSPI7 (memmap). R5f0Init brings up the SPI channel + resets the ADC + does the
 * reach self-check; R5f0UpdateState reads one conversion frame each (decimated)
 * tick into r5f_state, which PRU0 snapshots into the log ring.
 *
 * Prereqs on the Linux side before running: McSPI7 clocked (runtime-PM pinned,
 * /dev/spidevX left unopened) and an external CLKIN on the ADC (fMOD = fCLKIN/2).
=============================================================================*/
#include "r5floop.h"
#include "mcspi_j721e.h"
#include "gpio_j721e.h"
#include "ads131m08.h"

#define ADC_BASE     MCSPI7_BASE   // pru0_0 bus = McSPI7 (0x02170000), P9.29/30/31
#define ADC_CLKDIV   200u          // SCLK divider (verify on scope; conservative)
#define ADC_CS_BASE  GPIO1_BASE    // manual CS: P9.28 = main_gpio1 line 11
#define ADC_CS_BIT   (1u << 11)

void R5f0Init(pru_mem_t* mem) {
  // HL_REV != 0 proves the R5F reached McSPI7 through the interconnect/firewall.
  mem->s->r5f_state.hlrev = mcspiHlRev(ADC_BASE);
  // Configure SPI (24-bit, mode 1) + manual CS GPIO + RESET the ADC. (The ID it
  // returns lands in `response` until the first conversion frame overwrites it.)
  mem->s->r5f_state.response = ads131Init(ADC_BASE, ADC_CLKDIV, ADC_CS_BASE, ADC_CS_BIT);
}

void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io) {
  ads131_frame_t f;
  ads131ReadFrame(&f);
  for (int i = 0; i < 8; i++)
    io->s->adc[i] = f.ch[i];
  io->s->response = f.response;
  io->s->r5fvar++;
}

void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io) {}

void R5f0Cleanup(void) {}
