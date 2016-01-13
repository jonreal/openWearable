#include <stdint.h>

#include "soc_AM33XX.h"
#include "hw_types.h"
#include "spidriver.h"

void spiInit(void)
{
  /* TODO: param */
  uint16_t clkdiv = 200;

  /* CM_PER_SPI1_CLKCTRL: MODULEMODE = 0x2
   * This just enables the SPI1 module */
  HWREG(0x44E00050) = (0x2);

  /* MCSPI_SYSCONFIG:  Reset Module */
  HWREG(SOC_SPI_1_REGS + 0x110) = (0x1 << 1);

  /* MCSPI_SYSSTATUS: Poll for Reset done flag */
  while( (HWREG(SOC_SPI_1_REGS + 0x114) & 0x1) != 0x1) {}

  /* MCSPI_MODULCTRL:   FDAA = 0x0 - FIFO data in MCSPI_(TX/RX)(i)
   *                    MOA  = 0x0 - multi word disabled
   *                    INITDLY = 0x0 - no delay
   *                    SYSTEM_TEST = 0x0 - Functional mode
   *                    MS = 0x0 - master mode
   *                    PIN34 = 0x0 - SPIEN used as CS
   *                    SINGLE = 0x0 - multi-channel */
  HWREG(SOC_SPI_1_REGS + 0x128) = 0x00000000;

  /* MCSPI_SYSCONFIG: CLOCKACTIVITY = 0x3 - ocp and func. clocks maintained
   *                  SIDLEMODE = 0x2 - smart idle
   *                  AUTOIDLE = 0x1 - auto idle */
  HWREG(SOC_SPI_1_REGS + 0x110) = (0x3 << 8) | (0x1 << 3) | 0x0;

  /* MCSPI_IQRSTATUS: reset all interrupts */
  HWREG(SOC_SPI_1_REGS + 0x118) = 0xFFFFFFFF;

  /* MCSPI_IQRENABLE: none */
  HWREG(SOC_SPI_1_REGS + 0x11C) = 0x00000000;

  /* MCSPI_CH0CONF:   CLKG = 0x1 - a clock cycle granularity
   *                  FFER = 0x1 - enable FIFO for RX
   *                  FFEW = 0x1 - enable FIFO for TX
   *                  TCS = 0x0  - CS timing 0 cycles
   *                  SBPOL = 0x0 - start bit polarity
   *                  SBE = 0x0 - default TX length
   *                  SPIENSLV = 0x0 - Detection enabled only on SPIEN[0]
   *                  FORCE = 0x0
   *                  TURBO = 0x0
   *                  IS = 0x0 - Input (RX) SPIDAT[0]
   *                  DPE1 = 0x0 - Output (TX) SPIDAT[1]
   *                  DPE0 = 0x1 - No TX on SPIDAT[0]
   *                  DMAR = 0x0 - No DMA (RX)
   *                  DMAW = 0x0 - No DMA (TX)
   *                  TRM = 0x0 - TX/RX mode
   *                  WL = 0x7 - word length of 8-bits
   *                  EPOL = 0x1 - CS low for active state
   *                  CLKD = 0x4 - divide by 16
   *                  POL = 0x0
   *                  PHA = 0x0; */

  //HWREG(SOC_SPI_1_REGS + 0x12C) = (0x1 << 29) | (0x1 << 28) | (0x1 << 27)
  //HWREG(SOC_SPI_1_REGS + 0x12C) = (0x1 << 29) | (0x1 << 28) | (0x1 << 27)
  HWREG(SOC_SPI_1_REGS + 0x140) = (0x1 << 29) | (0x1 << 28) | (0x1 << 27)
          | (0x3 << 25) | (0x0 << 24) | (0x0 << 23) | (0x0 << 21) | (0x0 << 20)
          | (0x0 << 19) | (0x0 << 18) | (0x0 << 17) | (0x1 << 16) | (0x0 << 15)
          | (0x0 << 14) | (0x0 << 12) | (0x7 << 7) | (0x1 << 6)
          | ( (clkdiv & 0xF) << 2) | (0x0 << 1) | (0x0 << 0);

  /* MCSPI_CH0CTRL: EXTCLK =  clock divider extension */
  //HWREG(SOC_SPI_1_REGS + 0x134) = ( ((clkdiv >> 4) & 0xFF) << 8);
  HWREG(SOC_SPI_1_REGS + 0x148) = ( ((clkdiv >> 4) & 0xFF) << 8);

  HWREG(SOC_SPI_1_REGS + 0x148) |= 0x1;

}

uint8_t spiXfer(uint8_t tx)
{
  uint8_t rtn = 0;

  /* MCSPI_CH0CTRL: Enable SPI ch0 */
  //HWREG(SOC_SPI_1_REGS + 0x134) |= 0x1;

  //HWREG(SOC_SPI_1_REGS + 0x148) |= 0x1;

  /* MCSPI_TX0: Write payload word to TX FIFO ch0 */
  //HWREG(SOC_SPI_1_REGS + 0x138) = tx;
  HWREG(SOC_SPI_1_REGS + 0x14C) = tx;

  /* MCSPI_CH0STAT: Poll for EOT (end of transmission) */
  //while( (HWREG(SOC_SPI_1_REGS + 0x130) & (1 << 2)) == 0) {}
  while( (HWREG(SOC_SPI_1_REGS + 0x144) & (1 << 2)) == 0) {}

  /* MCSPI_RX0: Read word */
  //rtn = (uint8_t) HWREG(SOC_SPI_1_REGS + 0x13C);
  rtn = (uint8_t) HWREG(SOC_SPI_1_REGS + 0x150);

  /* MCSPI_CH0CTRL: Disable spi ch0 */
  //HWREG(SOC_SPI_1_REGS + 0x134) &= ~(0x1);
 // HWREG(SOC_SPI_1_REGS + 0x148) &= ~(0x1);

  return rtn;
}

void spiCleanUp(void)
{

 HWREG(SOC_SPI_1_REGS + 0x148) &= ~(0x1);

  /* Release clock */
  HWREG(0x44E00050) = (0x0);
}

