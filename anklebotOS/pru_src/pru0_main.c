#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

#include "imu_mpu9150.h"
//#include "encoder.h"


/* Prototypes -------------------------------------------------------------- */
void initialize(void);
void initMemory(void);
void updateState(uint32_t cnt, uint8_t bi, uint8_t si);
void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
void updateCounters(uint32_t *cnt, uint8_t *bi, uint8_t *si);
void cleanup(void);
void debugPinHigh(void);
void debugPinLow(void);


void iepTimerInit(uint32_t count);
void iepInterruptInit(void);
void startTimer(void);
void clearTimerFlag(void);
void clearIepInterrupt(void);
void pru0ToArmInterrupt(void);
//void spiInit(void);
//uint8_t spiXfer(uint8_t tx);
//void sampleEncoder(volatile uint16_t *pos);
void adcInit(void);
void sampleAdc(volatile uint16_t adc[8]);
void adcDisable(void);

/* Globals ----------------------------------------------------------------- */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Constant Table */
volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

/* Shared memory pointer */
shared_mem_t *p;

/* Param pointer */
param_mem_t *param;

/* FF pointer */
ff_mem_t *ff;

/* Debug Buffer */
volatile uint32_t *debugBuffer;

/* Main ---------------------------------------------------------------------*/
int main(void)
{
  uint32_t cnt = 0;
  uint8_t stateIndx = 0;
  uint8_t buffIndx = 0;

  initialize();

  /*** Wait for host interrupt ***/
  while( (__R31 & HOST0_MASK) == 0){}

  clearIepInterrupt();
  startTimer();

  /*** Loop ***/
  while(1){

    /* Poll for IEP timer interrupt */
    while((CT_INTC.SECR0 & (1 << 7)) == 0){}

    clearTimerFlag();

    debugPinHigh();

    /* Shadow enable bit */
    p->cntrl_bit.shdw_enable = p->cntrl_bit.enable;

    /* Update State */
    updateState(cnt, buffIndx, stateIndx);

    /* Set done bit (update state done) */
    p->cntrl_bit.pru0_done = 1;

    /* Poll for pru1 to be done (control update done) */
    while(!(p->cntrl_bit.pru1_done));

    /* Reset done bit */
    p->cntrl_bit.pru1_done = 0;

    /* Increment state index */
    updateCounters(&cnt ,&buffIndx, &stateIndx);

    /* Check enable bit */
    if(!(p->cntrl_bit.shdw_enable))
        break;

    clearIepInterrupt();

    debugPinLow();
 }

  debugPinLow();
  cleanup();
  __halt();
  return 0;
}

void initialize(void)
{
  /*** Init Pru ***/

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* PRU CTRL: CTR_EN = 0x1 - enable cycle counter */
  HWREG(PRU_CTRL_BASE) |= (1 << 3);

  /* Pin Mux */
  CT_CFG.GPCFG0 = 0;

  /*** Memory ***/
  initMemory();

  /* Add pru dependent peripheral init methods here */
  iepInterruptInit();
  iepTimerInit(param->frq_clock_ticks);
  clearIepInterrupt();
  adcInit();
//  spiInit();
//  encoderInit();
  imuInit();
}

void initMemory(void)
{
  void *ptr = NULL;

  /* Memory map for shared memory */
  ptr = (void *)PRU_L_SHARED_DRAM;
  p = (shared_mem_t *) ptr;

  /* Memory map for parameters (pru0 DRAM)*/
  ptr = (void *) PRU_DRAM;
  param = (param_mem_t *) ptr;

  /* Memory map for feedforward lookup table (pru1 DRAM)*/
  ptr = (void *) PRU_OTHER_DRAM;
  ff = (ff_mem_t *) ptr;

  /* Point global debug buffer */
  debugBuffer = &(param->debugBuffer[0]);
}


void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  /* Use cycle counts as timestamp (for debugging) */
//  p->state[bi][si].timeStamp = HWREG(PRU_CTRL_BASE + 0xC);

  p->state[bi][si].timeStamp = cnt;

  sampleAdc(p->state[bi][si].adc);

  imuSample(p->state[bi][si].imu);

  //encoderSample(&p->state[bi][si].anklePos);

  p->state[bi][si].ankleVel = 0xAAAA;
}

void updateControl(uint32_t cnt, uint8_t bi, uint8_t si)
{

}

void updateCounters(uint32_t *cnt, uint8_t *bi, uint8_t *si)
{
  (*cnt)++;
  (*si)++;
  if(*si == SIZE_OF_BUFFS){
    *si = 0;

    /* Set buffer full bit */
    if(*bi == 0)
      p->cntrl_bit.buffer0_full = 1;
    else
      p->cntrl_bit.buffer1_full = 1;

    (*bi)++;
    if(*bi == NUM_OF_BUFFS){
      *bi = 0;
    }
  }
}

void cleanup(void)
{
  /* Add pru dependent peripheral cleanup methods here */
  adcDisable();
  imuCleanUp();
//  spiCleanUp();
//  encoderCleanUp();

  /* Clear all interrupts */
  clearIepInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
}

void debugPinHigh(void)
{
  __R30 |= (1 << PRU0_DEBUG_PIN);
}

void debugPinLow(void)
{
  __R30 &= ~(1 << PRU0_DEBUG_PIN);
}

//void spiInit(void)
//{
//  /* TODO: param */
//  uint16_t clkdiv = 200;
//
//  /* CM_PER_SPI1_CLKCTRL: MODULEMODE = 0x2
//   * This just enables the SPI1 module */
//  HWREG(0x44E00050) = (0x2);
//
//  /* MCSPI_SYSCONFIG:  Reset Module */
//  HWREG(SPI1_BASE + 0x110) = (0x1 << 1);
//
//  /* MCSPI_SYSSTATUS: Poll for Reset done flag */
//  while( (HWREG(SPI1_BASE + 0x114) & 0x1) == 0) {}
//
//  /* MCSPI_MODULCTRL:   FDAA = 0x0 - FIFO data in MCSPI_(TX/RX)(i)
//   *                    MOA  = 0x0 - multi word disabled
//   *                    INITDLY = 0x0 - no delay
//   *                    SYSTEM_TEST = 0x0 - Functional mode
//   *                    MS = 0x0 - master mode
//   *                    PIN34 = 0x0 - SPIEN used as CS
//   *                    SINGLE = 0x0 - multi-channel */
//  HWREG(SPI1_BASE + 0x128) = 0x00000000;
//
//  /* MCSPI_SYSCONFIG: CLOCKACTIVITY = 0x3 - ocp and func. clocks maintained
//   *                  SIDLEMODE = 0x2 - smart idle
//   *                  AUTOIDLE = 0x1 - auto idle */
//  HWREG(SPI1_BASE + 0x110) = (0x3 << 8) | (0x2 << 3) | 0x1;
//
//  /* MCSPI_IQRSTATUS: reset all interrupts */
//  HWREG(SPI1_BASE + 0x118) = 0xFFFFFFFF;
//
//  /* MCSPI_IQRENABLE: none */
//  HWREG(SPI1_BASE + 0x11C) = 0x00000000;
//
//  /* MCSPI_CH0CONF:   CLKG = 0x1 - a clock cycle granularity
//   *                  FFER = 0x1 - enable FIFO for RX
//   *                  FFEW = 0x1 - enable FIFO for TX
//   *                  TCS = 0x0  - CS timing 0 cycles
//   *                  SBPOL = 0x0 - start bit polarity
//   *                  SBE = 0x0 - default TX length
//   *                  SPIENSLV = 0x0 - Detection enabled only on SPIEN[0]
//   *                  FORCE = 0x0
//   *                  TURBO = 0x0
//   *                  IS = 0x0 - Input (RX) SPIDAT[0]
//   *                  DPE1 = 0x0 - Output (TX) SPIDAT[1]
//   *                  DPE0 = 0x1 - No TX on SPIDAT[0]
//   *                  DMAR = 0x0 - No DMA (RX)
//   *                  DMAW = 0x0 - No DMA (TX)
//   *                  TRM = 0x0 - TX/RX mode
//   *                  WL = 0x7 - word length of 8-bits
//   *                  EPOL = 0x1 - CS low for active state
//   *                  CLKD = 0x4 - divide by 16
//   *                  POL = 0x0
//   *                  PHA = 0x0; */
//  HWREG(SPI1_BASE + 0x12C) = (0x1 << 29) | (0x1 << 28) | (0x1 << 27)
//          | (0x3 << 25) | (0x0 << 24) | (0x0 << 23) | (0x0 << 21) | (0x0 << 20)
//          | (0x0 << 19) | (0x0 << 18) | (0x0 << 17) | (0x1 << 16) | (0x0 << 15)
//          | (0x0 << 14) | (0x0 << 12) | (0x7 << 7) | (0x1 << 6)
//          | ( (clkdiv & 0xF) << 2) | (0x0 << 1) | (0x0 << 0);
//
//  /* MCSPI_CH0CTRL: EXTCLK =  clock divider extension */
//  HWREG(SPI1_BASE + 0x134) = ( ((clkdiv >> 4) & 0xFF) << 8);
//}
//
//uint8_t spiXfer(uint8_t tx)
//{
//  uint8_t rtn = 0;
//
//  /* MCSPI_CH0CTRL: Enable SPI ch0 */
//  HWREG(SPI1_BASE + 0x134) |= 0x1;
//
//  /* MCSPI_TX0: Write payload word to TX FIFO ch0 */
//  HWREG(SPI1_BASE + 0x138) = tx;
//
//  /* MCSPI_CH0STAT: Poll for EOT (end of transmission) */
//  while( (HWREG(SPI1_BASE + 0x130) & (1 << 2)) == 0) {}
//
//  /* MCSPI_RX0: Read word */
//  rtn = (uint8_t) HWREG(SPI1_BASE + 0x13C);
//
//  /* MCSPI_CH0CTRL: Disable spi ch0 */
//  HWREG(SPI1_BASE + 0x134) &= ~(0x1);
//
//  return rtn;
//}





void sampleAdc(volatile uint16_t adc[8])
{
  volatile uint32_t *FIFO =  (uint32_t *) (ADC_BASE + 0x100);

  /* STEPENABLE: Enable STEP1-8 */
  HWREG(ADC_BASE + 0x54) = (1 << 8) | (1 << 7) | (1 << 6) | (1 << 5)
      | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1);

  /* IRQSTATUS: poll for interrupt */
  while( (HWREG(ADC_BASE + 0x28) & (1 << 2)) == 0){}

  /* Write to memory */
  adc[0] = FIFO[0] & 0xFFF;
  adc[1] = FIFO[1] & 0xFFF;
  adc[2] = FIFO[2] & 0xFFF;
  adc[3] = FIFO[3] & 0xFFF;
  adc[4] = FIFO[4] & 0xFFF;
  adc[5] = FIFO[5] & 0xFFF;
  adc[6] = FIFO[6] & 0xFFF;
  adc[7] = FIFO[7] & 0xFFF;

  /* IRQSTATUS: Clear all interrupts */
  HWREG(ADC_BASE + 0x28) = 0x7FF;
}


void adcInit(void)
{

  /* TODO make these params */
  uint8_t sampleDelay = 10;
  uint16_t openDelay = 100;
  uint8_t avrg = 0x2;
  uint16_t adc_clk_div = 0x2;

  /* CTRL:  StepConfig_WriteProtext_n_active_low = 0x1 - enable step config
   *        Step_ID_tag = 0x1 - store ch id tag in FIFO
   *        Enable = 0x0 - Disable ADC */
  HWREG(ADC_BASE + 0x40) = (0x1 << 2) | (0x1 << 1) | (0x0);

  /* FIFO0THRESHOLD: FIFO0_threshold_Level = 7 (8-1) */
  HWREG(ADC_BASE + 0xE8) = 0x7;

  /* STEPENABLE: Disable all steps */
  HWREG(ADC_BASE + 0x54) = 0x0000;

  /**** Step configs - All steps configured for fifo0 ****/

  /* STEPCONFIG1: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x0 - ch1
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x64) = (0x0 << 26) | (0x0 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY1:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x68) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG2: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x1 - ch2
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x6C) = (0x0 << 26) | (0x1 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY2:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x70) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG3: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x2 - ch3
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x74) = (0x0 << 26) | (0x2 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY3:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x78) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG4: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x3 - ch4
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x7C) = (0x0 << 26) | (0x3 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY4:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x80) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG5: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x4 - ch5
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x84) = (0x0 << 26) | (0x4 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY5:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x88) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG6: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x5 - ch6
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x8C) = (0x0 << 26) | (0x5 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY6:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x90) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG7: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x6 - ch7
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x94) = (0x0 << 26) | (0x6 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY7:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0x98) = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG8: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x7 - ch7
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(ADC_BASE + 0x9C) = (0x0 << 26) | (0x7 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY8:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(ADC_BASE + 0xA0) = (sampleDelay << 24) | openDelay;

  /**** Turn off other steps ****/

  /* STEPCONFIG9: off */
  HWREG(ADC_BASE + 0xA4) = 0x0;

  /* STEPCONFIG10: off */
  HWREG(ADC_BASE + 0xAC) = 0x0;

  /* STEPCONFIG11: off */
  HWREG(ADC_BASE + 0xB4) = 0x0;

  /* STEPCONFIG12: off */
  HWREG(ADC_BASE + 0xBC) = 0x0;

  /* STEPCONFIG13: off */
  HWREG(ADC_BASE + 0xC4) = 0x0;

  /* STEPCONFIG14: off */
  HWREG(ADC_BASE + 0xCC) = 0x0;

  /* STEPCONFIG15: off */
  HWREG(ADC_BASE + 0xD4) = 0x0;

  /* STEPCONFIG16: off */
  HWREG(ADC_BASE + 0xDC) = 0x0;

  /* SYSCONFIG: IdleMode = 0x2 - Smart-Idel Mode */
  HWREG(ADC_BASE + 0x10) = (0x2 << 2);

  /* IRQENABLE_SET: FIFO0_Threshold = 0x1 - enable FIFO int */
  HWREG(ADC_BASE + 0x2C) = (1 << 2);

  /* ADC_CLKDIV: ADC_ClkDiv = 0x3 (divide by 4 (4-1 = 3)  */
  HWREG(ADC_BASE + 0x4C) = adc_clk_div;

  /* CTRL: Enable = 0x1 */
  HWREG(ADC_BASE + 0x40) |= 0x1;

  /* IRQSTATUS: Clear all interrupts */
  HWREG(ADC_BASE + 0x28) = 0x7FF;
}


void iepTimerInit(uint32_t count)
{
  /*** IEP Timer Setup ***/

  /* Enable ocp_clk */
  CT_CFG.IEPCLK_bit.OCP_EN = 1;

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  /* Reset Count register */
  CT_IEP.TMR_CNT = 0x0;

  /* Clear overflow status register */
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  /* Set compare value */
  CT_IEP.TMR_CMP0 = count;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  /* Disable compensation */
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  /* Enable CMP0 and reset on event */
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;
}

void iepInterruptInit(void)
{
  /*** System event 7 Interrupt -> Host 1 ***/

  /* Disable Global enable interrupts */
  CT_INTC.GER = 0;

  /* Clear any pending PRU-generated events */
  __R31 = 0x00000000;

  /* Map event 7 to channel 2 */
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  /* Map Channel 2 to host 2 */
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  /* Clear the status of all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* Enable event 7 */
  CT_INTC.EISR = 7;

  /* Enable Host interrupt 1 */
  CT_INTC.HIEISR |= (1 << 0);

  /* Gloablly enable interrupts */
  CT_INTC.GER = 1;
}

void startTimer(void)
{
   /* Start Timer */
//  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 1;
  CT_IEP.TMR_GLB_CFG = 0x11;
}

void clearTimerFlag(void)
{
    /* Clear compare status */
    CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
}

void pru0ToArmInterrupt(void)
{
  __R31 = PRU0_ARM_INT;
}

void clearIepInterrupt(void)
{
    /* Clear interrupt's status */
    CT_INTC.SECR0 = (1<<7);
    __R31 = 0x00000000;
}

void adcDisable(void)
{
  /* CTRL: Enable = 0x0*/
  HWREG(ADC_BASE + 0x40) &= ~(0x1);
}

