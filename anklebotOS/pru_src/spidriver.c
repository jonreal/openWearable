#include <stdint.h>

#include "soc_AM33XX.h"
#include "hw_types.h"
#include "spidriver.h"

void spiInit(void)
{

  uint16_t clkdiv_ch0 = 5;
  uint16_t clkdiv_ch1 = 200;

  // CM_PER_SPI1_CLKCTRL: MODULEMODE = 0x2
  // This just enables the SPI1 module
  HWREG(0x44E00050) = (0x2);

  // MCSPI_SYSCONFIG:  Reset Module
  HWREG(SOC_SPI_1_REGS + 0x110) = (0x1 << 1);

  // MCSPI_SYSSTATUS: Poll for Reset done flag
  while( (HWREG(SOC_SPI_1_REGS + 0x114) & 0x1) != 0x1) {}

  // MCSPI_MODULCTRL:   FDAA = 0x0 - FIFO data in MCSPI_(TX/RX)(i)
  //                    MOA  = 0x0 - multi word disabled
  //                    INITDLY = 0x0 - no delay
  //                    SYSTEM_TEST = 0x0 - Functional mode
  //                    MS = 0x0 - master mode
  //                    PIN34 = 0x0 - SPIEN used as CS
  //                    SINGLE = 0x0 - multi-channel
  HWREG(SOC_SPI_1_REGS + 0x128) = 0x00000000;

  // MCSPI_SYSCONFIG: CLOCKACTIVITY = 0x3 - ocp and func. clocks maintained
  //                  SIDLEMODE = 0x2 - smart idle
  //                  AUTOIDLE = 0x1 - auto idle
  HWREG(SOC_SPI_1_REGS + 0x110) = (0x3 << 8) | (0x1 << 3) | 0x0;

  // MCSPI_IQRSTATUS: reset all interrupts
  HWREG(SOC_SPI_1_REGS + 0x118) = 0xFFFFFFFF;

  // MCSPI_IQRENABLE: none
  HWREG(SOC_SPI_1_REGS + 0x11C) = 0x00000000;

  // -- CHANNEL 0 -------------------------------------------------------------
  //
  // Set up for LSM6DS3.
  // Word Length - 16-bit
  // POL = 1, PHA = 1
  // --------------------------------------------------------------------------

  // MCSPI_CH0CONF:   CLKG = 0x1 - a clock cycle granularity
  //                  TSC = 0x3 - clocks between CS
  //                  DPE0 = 0x1 - No TX on SPIDAT[0]
  //                  WL = 0xF - word length of 16-bits
  //                  EPOL = 0x1 - CS low for active state
  //                  CLKD = 0x4 - divide by 16
  //                  POL = 0x1 - CLK low for active
  //                  PHA = 0x1 - Even Edges
  HWREG(SOC_SPI_1_REGS + 0x12C) = (0x1 << 29) | (0x0 << 25) | (0x1 << 16)
                                | (0xF << 7) | (0x1 << 6)
                                | ( (clkdiv_ch0 & 0xF) << 2) | (0x1 << 1) | (0x1);

  // MCSPI_CH0CTRL:   EXTCLK =  clock divider extension
  //                  EN = 0x1 - enable channel 0
  HWREG(SOC_SPI_1_REGS + 0x134) = ( ((clkdiv_ch0 >> 4) & 0xFF) << 8) | 0x1;

  // -- CHANNEL 1 -------------------------------------------------------------
  //
  // Set up for atm encoder.
  // Word Length - 8-bit
  // POL = 0, PHA = 0
  // --------------------------------------------------------------------------

  // MCSPI_CH1CONF:   CLKG = 0x1 - a clock cycle granularity
  //                  DPE0 = 0x1 - No TX on SPIDAT[0]
  //                  WL = 0x7 - word length of 8-bits
  //                  EPOL = 0x1 - CS low for active state
  //                  CLKD = 0x4 - divide by 16
  HWREG(SOC_SPI_1_REGS + 0x140) = (0x1 << 29) | (0x3 << 25) | (0x1 << 16)
                                | (0x7 << 7) | (0x1 << 6)
                                | ( (clkdiv_ch1 & 0xF) << 2);

  // MCSPI_CH1CTRL:   EXTCLK =  clock divider extension
  //                  EN = 0x1 - enable channel 1
  HWREG(SOC_SPI_1_REGS + 0x148) = ( ((clkdiv_ch1 >> 4) & 0xFF) << 8) | 0x1;

}

uint16_t spiXfer(uint8_t channel, uint16_t tx)
{
  // IMU -- WL: 16-bits
  if (channel == 0){
    // MCSPI_TX0: Write payload to tx buffer
    HWREG(SOC_SPI_1_REGS + 0x138) = tx;

    // MCSPI_CH0STAT: Poll End of Transmission
    while( (HWREG(SOC_SPI_1_REGS + 0x130) & (1 << 2)) == 0) {}

    // MCSPI_RX0: Read Word
    return (uint16_t) HWREG(SOC_SPI_1_REGS + 0x13C);
  }

  // Encoder -- WL: 8-bits
  else if (channel == 1) {
    uint8_t tx_8 = (uint8_t) tx;

    // MCSPI_TX1: Write payload to tx buffer
    HWREG(SOC_SPI_1_REGS + 0x14C) = tx_8;

    // MCSPI_CH1STAT: Poll End of Transmission
    while( (HWREG(SOC_SPI_1_REGS + 0x144) & (1 << 2)) == 0) {}

    // MCSPI_RX1: Read Word
    return (uint8_t) HWREG(SOC_SPI_1_REGS + 0x150);
  }

  return 0;
}

void spiCleanUp(void)
{
  // Disable both channels
  HWREG(SOC_SPI_1_REGS + 0x134) &= ~(0x1);
  HWREG(SOC_SPI_1_REGS + 0x148) &= ~(0x1);

  // Release clock
  HWREG(0x44E00050) = (0x0);
}

