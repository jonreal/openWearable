#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include <rsc_table_am335x_pru.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;

typedef struct{
  volatile uint32_t timeStamp;
  volatile uint32_t ch[7];
} adc_t;

/* Interrupt controller */
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));

/* PRU CFG pg. 273 */
volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));

/* PRU IEP used for timing */
volatile far pruIep CT_IEP __attribute__((cregister("PRU_IEP", near), peripheral));

int main(void)
{
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /*************************
   * IEP timer compare setup
   ************************/

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  /* Reset Count register */
  CT_IEP.TMR_CNT = 0x0;

  /* Clear overflow status register */
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  /* Set compare value */
  CT_IEP.TMR_CMP0 = 0xFFFFFFF;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  /* Disable compensation */
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  /* Enable CMP0 and resest on event */
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;

  /* Clear the status of all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* END IEP Setup */

  /* Pointer to shared memory */
  void *mem = (void *) PRU_SHARED_DRAM;

  /* Struct pointer */
  adc_t *adc = (adc_t*) mem;

  /* Start Counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 1;

  int cnt = 0;

  while(1){

    /* Poll for IEP timer interrupt */
    while ((CT_INTC.SECR0 & (1 << 7)) == 0) {}

    /* Clear compare status */
    CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

    /* Sample ADC */
    adc->timeStamp = __R30;
    adc->ch[0] = 1;
    adc->ch[1] = 2;
    adc->ch[2] = 3;
    adc->ch[3] = 4;
    adc->ch[4] = 5;
    adc->ch[5] = 8;
    adc->ch[6] = 7;
    cnt++;

    /* Send Host Interrupt */
    __R31 = 35;

    /* Clear interrupt's status */
    CT_INTC.SECR0 = (1 << 7);
 }

  /* Clear all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  __halt();
  return 0;
}
