#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

//#include "imu_mpu9150.h"
#include "adcdriver.h"
#include "gaitPhase.h"
#include "viconsync.h"
#include "filter.h"

/* Local Param Struct (mirror) --------------------------------------------- */
typedef struct{
}local_t;

/* Prototypes -------------------------------------------------------------- */
void initialize(void);
void initMemory(void);
void updateLocalParams(void);
void updateState(uint32_t cnt, uint8_t bi, uint8_t si);
void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
void updateCounters(uint32_t *cnt, uint8_t *bi, uint8_t *si);
void cleanUp(void);
void debugPinHigh(void);
void debugPinLow(void);

void iepTimerInit(uint32_t count);
void iepInterruptInit(void);
void startTimer(void);
void clearTimerFlag(void);
void clearIepInterrupt(void);
void pru0ToArmInterrupt(void);

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

/* LookUp tables */
lookUp_mem_t *lookUp;

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

    /* Mirror params */
    updateLocalParams();

    /* Reset Gait phase */
    if(p->cntrl_bit.resetGaitPhase){
      p->cntrl_bit.gaitPhaseReady = 0;
      resetGaitPhase();
      p->cntrl_bit.resetGaitPhase = 0;
    }

    /* Update State */
    updateState(cnt, buffIndx, stateIndx);

    /* Check to see if GaitPhase Ready */
    if( !(p->cntrl_bit.gaitPhaseReady) && (isGaitPhaseReady()) )
      p->cntrl_bit.gaitPhaseReady = 1;

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
  cleanUp();
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

  /* Init timer */
  iepInterruptInit();
  iepTimerInit(param->frq_clock_ticks);
  clearIepInterrupt();

  /* Add pru dependent peripheral init methods here */
  adcInit();
  //imuInit();
  gaitPhaseInit(param);
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
  lookUp = (lookUp_mem_t *) ptr;

  /* Point global debug buffer */
  debugBuffer = &(param->debugBuffer[0]);
}

void updateLocalParams(void)
{
  gaitPhaseUpdateParams(param);
}

void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  uint16_t adc[8];

  p->state[bi][si].timeStamp = cnt;

  p->state[bi][si].sync = viconSync();

  adcSample_1(adc);
  adcSample_2(adc);

  /* Motor analog samples */
  p->state[bi][si].adc[0] = adc[0];
  p->state[bi][si].adc[1] = adc[1];

  /* Filter insoles */
  p->state[bi][si].adc[2] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter1, (int16_t)adc[2]);
  p->state[bi][si].adc[3] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter2, (int16_t)adc[3]);
  p->state[bi][si].adc[4] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter3, (int16_t)adc[4]);
  p->state[bi][si].adc[5] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter4, (int16_t)adc[5]);
  p->state[bi][si].adc[6] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter5, (int16_t)adc[6]);
  p->state[bi][si].adc[7] = (uint16_t)
                  firFixed(lookUp->firCoeff, param->filter6, (int16_t)adc[7]);

  //imuSample(p->state[bi][si].imu);

  gaitPhaseDetect(cnt,
                  &p->state[bi][si].gaitPhase,
                  &p->state[bi][si].avgPeriod,
                  &p->state[bi][si].heelStrikeCnt,
                  p->state[bi][si].adc);
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

void cleanUp(void)
{
  /* Add pru dependent peripheral cleanup methods here */
  adcCleanUp();
  //imuCleanUp();

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
  CT_IEP.TMR_CMP0 = (count-1);

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


