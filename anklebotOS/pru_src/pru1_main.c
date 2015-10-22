#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

#include "encoder.h"

#define PWM_SS2_BASE      0x48304000
#define EPWM2_BASE        0x48304200

/* Prototypes -------------------------------------------------------------- */
void initialize(void);
void initMemory(void);
void updateState(uint32_t cnt, uint8_t bi, uint8_t si);
void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
void updateCounters(uint32_t *cnt, uint8_t *bi, uint8_t *si);
void cleanup(void);
void debugPinHigh(void);
void debugPinLow(void);

void interruptInit(void);
void clearInterrupt(void);
void pru1ToArmInterrupt(void);

void pwmInit(void);
void disablePwm(void);
void setDuty(float duty, volatile uint16_t *motorDuty);

/* Globals ----------------------------------------------------------------- */
volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

/* State pointer */
shared_mem_t *p;

/* Param pointer */
param_mem_t *param;

/* Feedforward lookup table pointer */
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
  while( (__R31 & HOST1_MASK) == 0){}

  clearInterrupt();

  /*** Loop ***/
  while(1){

    /* Poll of IEP timer interrupt */
    while((CT_INTC.SECR0 & (1 << 7)) == 0);

    debugPinHigh();

    /* Update State */
    updateState(cnt, buffIndx, stateIndx);

    /* Poll for pru0 done bit (updating state)*/
    while(!(p->cntrl_bit.pru0_done));

    /* Reset pru0 done bit */
    p->cntrl_bit.pru0_done = 0;

    /* Update Control */
    updateControl(cnt, buffIndx, stateIndx);

    /* Set done bit (control update done) */
    p->cntrl_bit.pru1_done = 1;

    /* Increment state index */
    updateCounters(&cnt, &buffIndx, &stateIndx);

    /* Check enable bit */
    if(!(p->cntrl_bit.shdw_enable))
        break;

    /* Wait till interrupt is cleared */
    while(CT_INTC.SECR0 & (1 << 7));

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

  /* Pin Mux */
  CT_CFG.GPCFG0 = 0;

  /*** Memory ***/
  initMemory();

  /* Add pru dependent peripheral init methods here */
  encoderInit();
  pwmInit();
  interruptInit();
}

void updateCounters(uint32_t *cnt, uint8_t *bi, uint8_t *si)
{
  (*cnt)++;
  (*si)++;
  if(*si == SIZE_OF_BUFFS){
    *si = 0;
    (*bi)++;
    if(*bi == NUM_OF_BUFFS){
      *bi = 0;
    }
  }
}

void updateControl(uint32_t cnt, uint8_t bi, uint8_t si)
{

  setDuty(50.0, &(p->state[bi][si].motorDuty));

}

void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  encoderSample(&p->state[bi][si].anklePos);
}

void cleanup(void)
{
  /* Add pru dependent peripheral cleanup methods here */
  disablePwm();
  encoderCleanUp();

  /* Clear all interrupts */
  clearInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
}

void debugPinHigh(void)
{
  __R30 |= (1 << PRU1_DEBUG_PIN);
}

void debugPinLow(void)
{
  __R30 &= ~(1 << PRU1_DEBUG_PIN);
}

void initMemory(void)
{
  void *ptr = NULL;

  /* Memory map for shared memory */
  ptr = (void *)PRU_L_SHARED_DRAM;
  p = (shared_mem_t *) ptr;

  /* Memory map for parameters (pru0 DRAM)*/
  ptr = (void *) PRU_OTHER_DRAM;
  param = (param_mem_t *) ptr;

  /* Memory map for feedforward lookup table (pru1 DRAM)*/
  ptr = (void *) PRU_DRAM;
  ff = (ff_mem_t *) ptr;

  /* Point global debug buffer */
  debugBuffer = &(param->debugBuffer[0]);
}



void pwmInit(void)
{
  /* T_tbclk = PWM_CLK/CLK_DIV
   *
   * For up-down-count mode
   *    T_pwm = 2 x TBPRD x T_tbclk
   *    F_pwm = 1 / T_pwm
   *
   * PWM resolution
   *    Res (%) = F_pwm/F_sysclkout X 100
   *    Res (bits) = log2(F_pwm/F_sysclkout) */

  /* TODO: make this a param */
  uint16_t pwm_prd = 10000;

  /* Disable Global enable interrupts */
  CT_INTC.GER = 0;

  /**** PWM SS 2 registers *****/

  /* CLKCONFIG: ePWMCLK_EN = 0x1 - enable pwm clk
   *            eQEPCLKSTOP_REQ = 0x1 - disable qep clk
   *            eCAPCLKSTOP_REQ = 0x1 - disable cap clk */
  HWREG(PWM_SS2_BASE + 0x8) = (0x1 << 8) | (0x1 << 5) | (0x1 << 1);

  /**** EPWM registers *****/

  /* CMPCTL: SHDWAMODE = 0x1 - no shadow mode */
  HWREGH(EPWM2_BASE + 0xE) = (0x1 << 4);

  /* CMPA: compare reg */
  HWREGH(EPWM2_BASE + 0x12) = pwm_prd;

  /* CMPAHR: high res compare reg */
  HWREGH(EPWM2_BASE + 0x10) = 0x1;

  /* AQCTLA:  CAD = 0x1 - when cnt = compare (increasing) force EPWMxA high
   *          CAU = 0x2 - when cnt = compare (decreasing) force EPWMxA high */
  HWREGH(EPWM2_BASE + 0x16) = (0x1 << 6) | (0x2 << 4);

  /* TBCTL: PRDLD = 0x1 - shadow prd off
   *        CTRMODE = 0x2 - up-down-cnt mode */
  HWREGH(EPWM2_BASE) = (0x1 << 3) | 0x2;

  /* TBPRD: set period */
  HWREGH(EPWM2_BASE + 0xA) = pwm_prd;

  /*ETCLR: clear interrupts */
  HWREGH(EPWM2_BASE + 0x38) = 0x1;

  /*ETSEL: INTEN = 0x1 -  enable pwm interrupt */
  HWREGH(EPWM2_BASE + 0x32) = (0x1 << 3);

  /* Globaly enable interrupts */
  CT_INTC.GER = 1;
}

void setDuty(float duty, volatile uint16_t *motorDuty)
{
  /* Store duty command */
  *motorDuty = (uint16_t) (duty * 10.0);

  /* Calculate compare value */
  uint16_t cmpvalue = (uint16_t) (-100.0*duty + 10000.0);

  /* CMPA: compare register */
  HWREGH(EPWM2_BASE + 0x12) = cmpvalue;
}

void disablePwm(void)
{
  /* TODO: fix this */

  /* TBPRD: period = 0 */
  HWREGH(EPWM2_BASE + 0xA) = 0x1;

  /* CMPA: compare reg */
  HWREGH(EPWM2_BASE + 0x12) = 0x1;
}


void interruptInit(void)
{
  /* Disable Global enable interrupts */
  CT_INTC.GER = 0;

  /* Clear any pending PRU-generated events */
  __R31 = 0x00000000;

  /* Clear the status of all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* Gloablly enable interrupts */
  CT_INTC.GER = 1;
}

void pru1ToArmInterrupt(void)
{
  __R31 = PRU1_ARM_INT;
}

void clearInterrupt(void)
{
    __R31 = 0x00000000;
}

