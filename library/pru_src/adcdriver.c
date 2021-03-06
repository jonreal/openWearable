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

#include "adcdriver.h"
#include "soc_AM33XX.h"
#include "hw_types.h"
#include "tsc_adc.h"

void AdcInit(void)
{
  uint8_t sampleDelay = 0xF;
  uint16_t openDelay = 0xFF;
  uint8_t avrg = 0x2;
  uint16_t adc_clk_div = 0x0;

  /* CTRL:  StepConfig_WriteProtext_n_active_low = 0x1 - enable step config
   *        Step_ID_tag = 0x1 - store ch id tag in FIFO
   *        Enable = 0x0 - Disable ADC */
  HWREG(SOC_ADC_TSC_0_REGS + 0x40) = (0x1 << 2) | (0x1 << 1) | (0x0);

  /* FIFO0THRESHOLD: FIFO0_threshold_Level = 7 (8-1) */
  HWREG(SOC_ADC_TSC_0_REGS + 0xE8) = 0x7;

  /* STEPENABLE: Disable all steps */
  HWREG(SOC_ADC_TSC_0_REGS + 0x54) = 0x0000;

  /* IDLECONFIG */
  HWREG(SOC_ADC_TSC_0_REGS + 0x58) = (0x3 << 23) | (0x8 << 19) | (0x8 << 15)
                                   | (0x0 << 12);

  /**** Step configs - All steps configured for fifo0 ****/

  /* ------------------------------------------------------------------------*/
  /* STEP 1 -> CH0  ***/

  /* STEPCONFIG1: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x0 - ch0
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x64) = (0x0 << 26) | (0x0 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY1:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x68) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 2 -> CH1  ***/

  /* STEPCONFIG2: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x1 - ch1
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x6C) = (0x0 << 26) | (0x1 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY2:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x70) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 3 -> CH2 ***/

  /* STEPCONFIG3: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x2 - ch2
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x74) = (0x0 << 26) | (0x2 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY3:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x78) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 4 -> CH3 ***/

  /* STEPCONFIG4: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x3 - ch3
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x7C) = (0x0 << 26) | (0x3 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY4:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x80) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 5 -> CH4 ***/

  /* STEPCONFIG5: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x4 - ch4
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x84) = (0x0 << 26) | (0x4 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY5:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x88) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 6 -> CH5 ***/

  /* STEPCONFIG6: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x5 - ch5
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x8C) = (0x0 << 26) | (0x5 << 19) | (avrg << 2) | (0x0);

  /* STEPDELAY6:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x90) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 7 -> CH6 ***/

  /* STEPCONFIG7: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x6 - ch6
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x94) = (0x0 << 26) | (0x6 << 19)
                                    | (avrg << 2) | (0x0);

  /* STEPDELAY7:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0x98) = (sampleDelay << 24) | openDelay;

  /* ------------------------------------------------------------------------*/
  /* STEP 8 -> CH7 (VADC) ***/

  /* STEPCONFIG8: FIFO_select = 0x0
   *              SEL_INP_SWC_3_0 = 0x7 - ch4
   *              Averaging = 0x2 - 4 samples
   *              Mode = 0x0 - SW enabled, one-shot */
  HWREG(SOC_ADC_TSC_0_REGS + 0x9C) = (0x0 << 26) | (0x7 << 19)
                                    | (avrg << 2) | (0x0);

  /* STEPDELAY8:  SampleDelay = sampleDelay - number of clks to sample
   *              OpenDelay = openDelay - clks to wait */
  HWREG(SOC_ADC_TSC_0_REGS + 0xA0) = (sampleDelay << 24) | openDelay;

  /**** Turn off other steps ****/

  /* STEPCONFIG10: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xAC) = 0x0;

  /* STEPCONFIG11: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xB4) = 0x0;

  /* STEPCONFIG12: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xBC) = 0x0;

  /* STEPCONFIG13: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xC4) = 0x0;

  /* STEPCONFIG14: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xCC) = 0x0;

  /* STEPCONFIG15: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xD4) = 0x0;

  /* STEPCONFIG16: off */
  HWREG(SOC_ADC_TSC_0_REGS + 0xDC) = 0x0;

  /* SYSCONFIG: IdleMode = 0x0 - Force Idle Mode */
  HWREG(SOC_ADC_TSC_0_REGS + 0x10) = (0x0 << 2);

  /* IRQENABLE_SET: FIFO0_Threshold = 0x1 - enable FIFO int */
  HWREG(SOC_ADC_TSC_0_REGS + 0x2C) = (1 << 2);

  /* ADC_CLKDIV: ADC_ClkDiv = 0x3 (divide by 4 (4-1 = 3)  */
  HWREG(SOC_ADC_TSC_0_REGS + 0x4C) = adc_clk_div;

  /* CTRL: Enable = 0x1 */
  HWREG(SOC_ADC_TSC_0_REGS + 0x40) |= 0x1;

  /* IRQSTATUS: Clear all interrupts */
  HWREG(SOC_ADC_TSC_0_REGS + 0x28) = 0x7FF;
}

uint32_t AdcSampleChBits(uint8_t ch)
{
  volatile uint32_t *FIFO =  (uint32_t *) (SOC_ADC_TSC_0_REGS + 0x100);
  uint32_t rtn = 0;

  // FIFO0THRESHOLD: FIFO0_threshold_Level = 0 (1-1) 
  HWREG(SOC_ADC_TSC_0_REGS + 0xE8) = 0x0;

  // Enable steps:
  HWREG(SOC_ADC_TSC_0_REGS + 0x54) = (1 << (ch+1));

  // IRQSTATUS: poll for interrupt
  while( (HWREG(SOC_ADC_TSC_0_REGS + 0x28) & (1 << 2)) == 0){}

  // Write to memory
  rtn = (uint32_t) (FIFO[0] & 0xFFF);

  // IRQSTATUS: Clear all interrupts
  HWREG(SOC_ADC_TSC_0_REGS + 0x28) = 0x7FF;

  return rtn;
}

fix16_t AdcSampleChmV(uint8_t ch){
  return fix16_smul(fix16_from_int((int16_t)AdcSampleChBits(ch)),
                    FIX16_BITS2VOLTS);
}



//uint32_t adcSample_2(uint8_t ch)
//{
//  volatile uint32_t *FIFO =  (uint32_t *) (SOC_ADC_TSC_0_REGS + 0x100);
//
//  /* FIFO0THRESHOLD: FIFO0_threshold_Level = 2 (3-1) */
//  HWREG(SOC_ADC_TSC_0_REGS + 0xE8) = 0x2;
//
//  /* Enalbe steps: 3 - forcesenor1 ao1
//   *               4 - forcesenor1 ao2
//   *               5 - forcesenor1 ao3 */
//  HWREG(SOC_ADC_TSC_0_REGS + 0x54) = (1 << 5) | (1 << 4) | (1 << 3);
//
//  /* IRQSTATUS: poll for interrupt */
//  while( (HWREG(SOC_ADC_TSC_0_REGS + 0x28) & (1 << 2)) == 0){}
//
//  /* Write to memory */
//  adc[2] = (int16_t) (FIFO[0] & 0xFFF);
//  adc[3] = (int16_t) (FIFO[1] & 0xFFF);
//  adc[4] = (int16_t) (FIFO[2] & 0xFFF);
//
//  /* IRQSTATUS: Clear all interrupts */
//  HWREG(SOC_ADC_TSC_0_REGS + 0x28) = 0x7FF;
//
//  /* Mux pin high (Mux1) */
//  __R30 |= (1 << MUX_SEL_PIN);
//}
//
//void adcSample_3(int16_t adc[8])
//{
//  volatile uint32_t *FIFO =  (uint32_t *) (SOC_ADC_TSC_0_REGS + 0x100);
//
//  /* Mux pin high (Mux1) */
//  __R30 |= (1 << MUX_SEL_PIN);
//
//  __delay_cycles(250);
//
//  /* FIFO0THRESHOLD: FIFO0_threshold_Level = 3 (4-1) */
//  HWREG(SOC_ADC_TSC_0_REGS + 0xE8) = 0x3;
//
//  /* Enalbe steps: 6 - forcesenor2 ao1
//   *               7 - forcesenor2 ao2
//   *               8 - forcesenor2 ao3
//   *               9 - Vadc */
//  HWREG(SOC_ADC_TSC_0_REGS + 0x54) = (1 << 9) | (1 << 8) | (1 << 7)
//                                      | (1 << 6);
//
//  /* IRQSTATUS: poll for interrupt */
//  while( (HWREG(SOC_ADC_TSC_0_REGS + 0x28) & (1 << 2)) == 0){}
//
//  /* Write to memory */
//  adc[5] = (int16_t) (FIFO[0] & 0xFFF);
//  adc[6] = (int16_t) (FIFO[1] & 0xFFF);
//  adc[7] = (int16_t) (FIFO[2] & 0xFFF);
//
//  /* Vadc */
//  uint16_t temp = FIFO[3] & 0xFFF;
//
//  /* IRQSTATUS: Clear all interrupts */
//  HWREG(SOC_ADC_TSC_0_REGS + 0x28) = 0x7FF;
//
//  /* Mux pin low (Mux0) */
//  __R30 &= ~(1 << MUX_SEL_PIN);
//}

void AdcCleanup(void)
{
  /* Set to disable */
  HWREG(SOC_ADC_TSC_0_REGS + 0x40) &= ~0x1;
}

