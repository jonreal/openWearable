/* Copyright 2017 Jonathan Realmuto

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

#include <stdint.h>

#include "soc_AM33XX.h"
#include "hw_types.h"
#include "spidriver.h"

#define MCSPI_BASE           0x481A0000
#define MCSPI_CH0CONF        (MCSPI_BASE + 0x12C)
#define MCSPI_CH0CTRL        (MCSPI_BASE + 0x134)
#define MCSPI_TX0            (MCSPI_BASE + 0x138)
#define MCSPI_RX0            (MCSPI_BASE + 0x13C)
#define MCSPI_CH0STAT        (MCSPI_BASE + 0x130)

void spiInit(void)
{

  uint16_t clkdiv_ch0 = 400;
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
  // --------------------------------------------------------------------------

  // MCSPI_CH0CONF:
  HWREG(SOC_SPI_1_REGS + 0x12C) =
      (0x1 << 29)                 // CLKG, Clock div granularity
    | (0x1 << 28)                 // FFER, FIFO Rx enable
    | (0x1 << 27)                 // FFEW, FIFO Tx enable
    | (0x0 << 25)                 // TCS, Chip select time control
    | (0x0 << 24)                 // SBPOL, Start bit polarity
    | (0x0 << 23)                 // SBE, Start bit enable
    | (0x0 << 21)                 // SPIENSLV,
    | (0x0 << 20)                 // Force, Manual assert SPIEN
    | (0x0 << 19)                 // Turbo,
    | (0x0 << 18)                 // IS, Input select
    | (0x0 << 17)                 // DPE1, Tx enable on SPIDAT[1]
    | (0x1 << 16)                 // DPE0, Tx enable on SPIDAT[0]
    | (0x0 << 15)                 // DMAR, DMA read request
    | (0x0 << 14)                 // DMAW, DMA write request
    | (0x0 << 12)                 // TRM, Tx/RX modes
    | (0x17 << 7)                 // WL, word length
    | (0x1 << 6)                  // EPOL, SPIEN polarity
    | ( (clkdiv_ch0 & 0xF) << 2)  // CLKD, Freq div
    | (0x0 << 1)                  // POL
    | (0x1);                      // PHA

  // MCSPI_CH0CTRL:  EN = 0x1 - enable channel 0
 // HWREG(SOC_SPI_1_REGS + 0x134) =  0x1;
  HWREG(SOC_SPI_1_REGS + 0x134) = ( ((clkdiv_ch1 >> 4) & 0xFF) << 8) | 0x1;

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
  //HWREG(SOC_SPI_1_REGS + 0x140) = (0x1 << 29) | (0x3 << 25) | (0x1 << 16)
  //                              | (0x7 << 7) | (0x1 << 6)
  //                              | ( (clkdiv_ch1 & 0xF) << 2);

  //// MCSPI_CH1CTRL:   EXTCLK =  clock divider extension
  ////                  EN = 0x1 - enable channel 1
  //HWREG(SOC_SPI_1_REGS + 0x148) = ( ((clkdiv_ch1 >> 4) & 0xFF) << 8) | 0x1;

}

uint32_t spiXfer(uint8_t channel, uint32_t tx)
{
  // IMU -- WL: 16-bits
  if (channel == 0){
    // MCSPI_TX0: Write payload to tx buffer
    HWREG(SOC_SPI_1_REGS + 0x138) = tx;

    // MCSPI_CH0STAT: Poll End of Transmission
    while( (HWREG(SOC_SPI_1_REGS + 0x130) & (1 << 2)) == 0) {}

    // MCSPI_RX0: Read Word
    return (uint32_t) HWREG(SOC_SPI_1_REGS + 0x13C);
  }

  // Encoder -- WL: 8-bits
  else if (channel == 1) {

    // MCSPI_TX1: Write payload to tx buffer
    HWREG(SOC_SPI_1_REGS + 0x14C) = (uint8_t) tx;

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

void spiForceBeginXfer() {
  HWREG(SOC_SPI_1_REGS + 0x12C) |= (1 << 20);  // FORCE=1
  __delay_cycles(10);
}

void spiForceEndXfer() {
  HWREG(SOC_SPI_1_REGS + 0x12C) &= ~(1 << 20);
  __delay_cycles(10);
}

void spiMultiWordXfer(uint32_t* tx, uint32_t* rx, uint32_t len) {
    for (int i = 0; i < len; ++i) {
      while (!(HWREG(MCSPI_CH0STAT) & (1 << 1))) {} // TXS
      HWREG(MCSPI_TX0) = tx[i];

      while (!(HWREG(MCSPI_CH0STAT) & (1 << 0))) {} // RXS
      rx[i] = HWREG(MCSPI_RX0);
    }

}
