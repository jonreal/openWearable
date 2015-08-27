#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"

/* Prototypes -------------------------------------------------------------- */
static void updateState(uint32_t cnt, uint8_t bi, uint8_t si);
static inline void iepTimerInit(uint32_t count);
static inline void iepInterruptInit(void);
static inline void startTimer(void);
static inline void clearTimerFlag(void);
static inline void clearIepInterrupt(void);
static inline void pru0ToArmInterrupt(void);

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
//    updateState(cnt, buffIndx, stateIndx);

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

  /* Wait for Host interrupt */
//  while( (__R31 & HOST0_MASK) == 0){}

  /* Send acknowledgement */
//  pru0ToArmInterrupt();

  __halt();
  return 0;
}

static void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  p->state[bi][si].timeStamp = cnt;

  p->state[bi][si].ankle_pos = cnt;
  p->state[bi][si].ankle_vel = cnt;

  p->state[bi][si].adc_value[0] = cnt;
  p->state[bi][si].adc_value[1] = cnt;
  p->state[bi][si].adc_value[2] = cnt;
  p->state[bi][si].adc_value[3] = cnt;
  p->state[bi][si].adc_value[4] = cnt;
  p->state[bi][si].adc_value[5] = cnt;
  p->state[bi][si].adc_value[6] = 100;

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

