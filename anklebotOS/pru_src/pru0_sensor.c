#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"


#define ADC_BASE          0x44E0D000
#define ADC_EN_BIT_MASK   0x1FE

/* Prototypes -------------------------------------------------------------- */
static void updateState(uint32_t cnt, uint8_t bi, uint8_t si);
static inline void iepTimerInit(uint32_t count);
static inline void iepInterruptInit(void);
static inline void startTimer(void);
static inline void clearTimerFlag(void);
static inline void clearIepInterrupt(void);
static inline void pru0ToArmInterrupt(void);
static void adcInit(void);
static inline void adcEnable(void);
static inline void adcDisable(void);

/* Globals ----------------------------------------------------------------- */

volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

/* Shared memory pointer */
shared_mem_t *p;

/* Param pointer */
pru0_param_mem_t *param;

/* Main ---------------------------------------------------------------------*/
int main(void)
{
  void *ptr;
  volatile uint32_t cnt = 0;
  volatile uint8_t stateIndx = 0;
  volatile uint8_t buffIndx = 0;

  /*** Init Pru ***/

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Pin Mux */
  CT_CFG.GPCFG0 = 0;

  /*** Memory Setup ***/

  /* Local memory map for shared memory */
  ptr = NULL;
  ptr = (void *)PRU_L_SHARED_DRAM;
  p = (shared_mem_t *) ptr;

  /* Memory map for dram (params) */
  ptr = NULL;
  ptr = (void *) PRU_DRAM;
  param = (pru0_param_mem_t *) ptr;

  /*** Init timer ***/
  iepInterruptInit();
  iepTimerInit(param->frq_clock_ticks);
  clearIepInterrupt();

  /*** Init ADC ***/
  adcInit();


  /*** Wait for host interrupt ***/
  while( (__R31 & HOST0_MASK) == 0){}

  clearIepInterrupt();
  startTimer();

  /*** Sensor Loop ***/
  cnt = 0;
  while(1){

    /* Poll for IEP timer interrupt */
    while ( (CT_INTC.SECR0 & (1 << 7)) == 0){}

    clearTimerFlag();

    /* Pin High */
    __R30 |= (1<<0);

    /* Update State */
    updateState(cnt, buffIndx, stateIndx);

    __delay_cycles(1000);

    /* Increment state index */
    cnt++;
    stateIndx++;
    if(stateIndx == SIZE_OF_BUFFS){
      stateIndx = 0;
      buffIndx++;
      if(buffIndx == NUM_OF_BUFFS){
        buffIndx = 0;
      }
    }

    /* Set done bit to start control */
    p->flow_control_bit.sensorDone = 1;

    /* Check enable bit */
    if(!(p->flow_control_bit.enable)){
      p->flow_control_bit.exit = 1;
      break;
    }

    clearIepInterrupt();

    /* Pin Low */
    __R30 &= ~(1<<0);
 }

  /* Pin Low */
  __R30 &= ~(1<<0);
  __R30 = 0;

  /* Clear all interrupts */
  clearIepInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* CTRL: Disable ADC */
  uint32_t *pp = NULL;
  pp = (uint32_t *) (ADC_BASE + 0x40);
  *pp = 0x0;

  /* Wait for Host interrupt */
//  while( (__R31 & HOST0_MASK) == 0){}

  /* Send acknowledgement */
//  pru0ToArmInterrupt();

  __halt();
  return 0;
}

static void adcInit(void)
{

  uint8_t sampleDelay = 254;
  uint8_t openDelay = 100000;

  uint32_t *pp = NULL;

  /* CTRL: Disable ADC */
  pp = (uint32_t *) (ADC_BASE + 0x40);
  *pp = 0x0;

  /* CTRL: Enable step config, and ID tag */
  *pp |= 0x6;

  /* FIFO0THRESHOLD: set to 7 = (8-1) */
  pp = (uint32_t *) (ADC_BASE + 0xE8);
  *pp = 0x7;

  /* Disable all steps */
  pp = (uint32_t *) (ADC_BASE + 0x54);
  *pp = 0x0;

  /* All steps configured for fifo0 */

  /* STEPCONFIG1: Set to CH. 1, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x64);
  *pp = (0x0 << 19);

  /* STEPDELAY1: */
  pp = (uint32_t *) (ADC_BASE + 0x68);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG2: Set to CH. 2, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x6C);
  *pp = (0x1 << 19);

  /* STEPDELAY2: */
  pp = (uint32_t *) (ADC_BASE + 0x70);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG3: Set to CH. 3, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x74);
  *pp = (0x2 << 19);

  /* STEPDELAY3: */
  pp = (uint32_t *) (ADC_BASE + 0x78);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG4: Set to CH. 4, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x7C);
  *pp = (0x3 << 19);

  /* STEPDELAY4: */
  pp = (uint32_t *) (ADC_BASE + 0x80);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG5: Set to CH. 5, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x84);
  *pp = (0x4 << 19);

  /* STEPDELAY5: */
  pp = (uint32_t *) (ADC_BASE + 0x88);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG6: Set to CH. 6, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x8C);
  *pp = (0x5 << 19);

  /* STEPDELAY6: */
  pp = (uint32_t *) (ADC_BASE + 0x90);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG7: Set to CH. 7, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x94);
  *pp = (0x6 << 19);

  /* STEPDELAYd7: */
  pp = (uint32_t *) (ADC_BASE + 0x98);
  *pp = (sampleDelay << 24) | openDelay;

  /* STEPCONFIG8: Set to CH. 8, and avg. 16 samples */
  pp = (uint32_t *) (ADC_BASE + 0x9C);
  *pp = (0x7 << 19);

  /* STEPDELAY8: */
  pp = (uint32_t *) (ADC_BASE + 0xA0);
  *pp = (sampleDelay << 24) | openDelay;

  /* Turn off other steps */

  /* STEPCONFIG9: off */
  pp = (uint32_t *) (ADC_BASE + 0xA4);
  *pp = 0x0;

  /* STEPCONFIG10: off */
  pp = (uint32_t *) (ADC_BASE + 0xAC);
  *pp = 0x0;

  /* STEPCONFIG11: off */
  pp = (uint32_t *) (ADC_BASE + 0xB4);
  *pp = 0x0;

  /* STEPCONFIG12: off */
  pp = (uint32_t *) (ADC_BASE + 0xBC);
  *pp = 0x0;

  /* STEPCONFIG13: off */
  pp = (uint32_t *) (ADC_BASE + 0xC4);
  *pp = 0x0;

  /* STEPCONFIG14: off */
  pp = (uint32_t *) (ADC_BASE + 0xCC);
  *pp = 0x0;

  /* STEPCONFIG15: off */
  pp = (uint32_t *) (ADC_BASE + 0xD4);
  *pp = 0x0;

  /* STEPCONFIG16: off */
  pp = (uint32_t *) (ADC_BASE + 0xDC);
  *pp = 0x0;

  /* SYSCONFIG: Force Idle */
  pp = (uint32_t *) (ADC_BASE + 0x10);
  *pp = 0x0;

  /* IRQENABLE_SET: Interrput for FIFO0 Threshold */
  pp = (uint32_t *) (ADC_BASE + 0x2C);
  *pp = (1<<2);

  /* ADC_CLKDIV: 0 */
  pp = (uint32_t *) (ADC_BASE + 0x4C);
  *pp = 0x0;

  /* CTRL: Enable ADC */
  pp = (uint32_t *) (ADC_BASE + 0x40);
  *pp |= 0x1;

  /* ADC_IRQSTATUS: Reset Iterrupt */
  pp = (uint32_t *) (ADC_BASE + 0x28);
  *pp = (1<<2);
}

static void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  volatile uint32_t *STEPENABLE = (uint32_t *) (ADC_BASE + 0x54);
  volatile uint32_t *FIFO = (uint32_t *) (ADC_BASE + 0x100);
  volatile uint32_t *adc_irqstatus = (uint32_t *) (ADC_BASE + 0x28);

  /* Start ADC capture */
  *STEPENABLE = ADC_EN_BIT_MASK;

  /* Poll for interrupt */
  while( (*adc_irqstatus & (1<<2)) == 0){}

  p->state[bi][si].timeStamp = cnt;
  p->state[bi][si].adc_value[0] = FIFO[0] & 0xFFF;
  p->state[bi][si].adc_value[1] = FIFO[1] & 0xFFF;
  p->state[bi][si].adc_value[2] = FIFO[2] & 0xFFF;
  p->state[bi][si].adc_value[3] = FIFO[3] & 0xFFF;
  p->state[bi][si].adc_value[4] = FIFO[4] & 0xFFF;
  p->state[bi][si].adc_value[5] = FIFO[5] & 0xFFF;
  p->state[bi][si].adc_value[6] = FIFO[6] & 0xFFF;

  uint32_t temp0 = FIFO[7] & 0xFFF;

  /* Reset Iterrupt */
   *adc_irqstatus = (1<<2);

  /*
  p->state[bi][si].imu_value[0] = cnt;
  p->state[bi][si].imu_value[1] = cnt;
  p->state[bi][si].imu_value[2] = cnt;
  p->state[bi][si].imu_value[3] = cnt;
  p->state[bi][si].imu_value[4] = cnt;
  p->state[bi][si].imu_value[5] = cnt;
  p->state[bi][si].imu_value[6] = cnt;
  p->state[bi][si].imu_value[7] = cnt;
  p->state[bi][si].imu_value[8] = 99;

  p->state[bi][si].gaitPhase = 1;
  */
  
}

static inline void iepTimerInit(uint32_t count)
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

static inline void iepInterruptInit(void)
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
  CT_INTC.HIEISR |= (1<<0);

  /* Gloablly enable interrupts */
  CT_INTC.GER = 1;
}

static inline void startTimer(void)
{
   /* Start Timer */
//  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 1;
  CT_IEP.TMR_GLB_CFG = 0x11;
}

static inline void clearTimerFlag(void)
{
    /* Clear compare status */
    CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
}

static inline void pru0ToArmInterrupt(void)
{
  __R31 = PRU0_ARM_INT;
}

static inline void clearIepInterrupt(void)
{
    /* Clear interrupt's status */
    CT_INTC.SECR0 = (1<<7);
    __R31 = 0x00000000;
}


static inline void adcEnable(void)
{
  uint32_t *pp = NULL;

  /* CTRL: enable */
  pp = (uint32_t *) (ADC_BASE + 0x40);
  *pp |= 0x1;
}

static inline void adcDisable(void)
{
  uint32_t *pp = NULL;

  /* CTRL: enable */
  pp = (uint32_t *) (ADC_BASE + 0x40);
  *pp |= 0x0;
}


