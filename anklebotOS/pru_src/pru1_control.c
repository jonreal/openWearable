#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"

#define PWM_SS2_BASE      0x48304000
#define EPWM2_BASE        0x48304200

/* Prototypes -------------------------------------------------------------- */
void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
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
pru1_param_mem_t *param;

/* Main ---------------------------------------------------------------------*/
int main(void)
{
  void *ptr;
  uint32_t cnt = 0;
  uint8_t stateIndx = 0;
  uint8_t buffIndx = 0;

  /*** Init Pru ***/

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Pin Mux */
  CT_CFG.GPCFG0 = 0;

  /*** Memory Setup ***/

  /* Memory map for shared memory */
  ptr = (void *)PRU_L_SHARED_DRAM;
  p = (shared_mem_t *) ptr;

  /* Memory map for local memory*/
  ptr = (void *) PRU_DRAM;
  param = (pru1_param_mem_t *) ptr;

  /* Init pwm */
  pwmInit();

  /* Init interrupts */
  interruptInit();

  /* Wait for host interrupt */
  while( (__R31 & HOST1_MASK) == 0){}

  clearInterrupt();

  /*** Control Loop ***/
  while(1){

    /* Poll for sensor done bit */
    while( (p->flow_control_bit.sensorDone) == 0){}

    /* Reset sensor bit */
    p->flow_control_bit.sensorDone = 0;

    /* Pin High */
    __R30 |= (1 << PRU1_DEBUG_PIN);

    /* Update Control */
    updateControl(cnt, buffIndx, stateIndx);

    /* Increment state index */
    cnt++;
    stateIndx++;
    if(stateIndx == SIZE_OF_BUFFS){
      stateIndx = 0;
      buffIndx++;
      p->flow_control_bit.bufferFull = 1;
      if(buffIndx == NUM_OF_BUFFS){
        buffIndx = 0;
      }
    }

    /* Check for exit flag*/
    if(p->flow_control_bit.exit)
      break;

    /* Pin Low */
    __R30 &= ~(1 << PRU1_DEBUG_PIN);
  }

  /* Pin Low */
  __R30 &= ~(1 << PRU1_DEBUG_PIN);

  /* Disable pwm */
  disablePwm();

  /* Clear all interrupts */
  clearInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* Wait for Host Interrupt */
//  while( (__R31 & HOST1_MASK) == 0){}

//  __delay_cycles(1000);

  /* Send interrupt to ARM */
//  pru1ToArmInterrupt();

  __halt();
  return 0;
}

void updateControl(uint32_t cnt, uint8_t bi, uint8_t si)
{

  setDuty(100, &(p->state[bi][si].motorDuty));

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
  HWREG32(PWM_SS2_BASE + 0x8) = (0x1 << 8) | (0x1 << 5) | (0x1 << 1);

  /**** EPWM registers *****/

  /* CMPCTL: SHDWAMODE = 0x1 - no shadow mode */
  HWREG16(EPWM2_BASE + 0xE) = (0x1 << 4);

  /* CMPA: compare reg */
  HWREG16(EPWM2_BASE + 0x12) = pwm_prd;

  /* CMPAHR: high res compare reg */
  HWREG16(EPWM2_BASE + 0x10) = 0x1;

  /* AQCTLA:  CAD = 0x1 - when cnt = compare (increasing) force EPWMxA high
   *          CAU = 0x2 - when cnt = compare (decreasing) force EPWMxA high */
  HWREG16(EPWM2_BASE + 0x16) = (0x1 << 6) | (0x2 << 4);

  /* TBCTL: PRDLD = 0x1 - shadow prd off
   *        CTRMODE = 0x2 - up-down-cnt mode */
  HWREG16(EPWM2_BASE) = (0x1 << 3) | 0x2;

  /* TBPRD: set period */
  HWREG16(EPWM2_BASE + 0xA) = pwm_prd;

  /*ETCLR: clear interrupts */
  HWREG16(EPWM2_BASE + 0x38) = 0x1;

  /*ETSEL: INTEN = 0x1 -  enable pwm interrupt */
  HWREG16(EPWM2_BASE + 0x32) = (0x1 << 3);

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
  HWREG16(EPWM2_BASE + 0x12) = cmpvalue;
}

void disablePwm(void)
{
  /* TODO: fix this */

  /* TBPRD: period = 0 */
  HWREG16(EPWM2_BASE + 0xA) = 0x1;

  /* CMPA: compare reg */
  HWREG16(EPWM2_BASE + 0x12) = 0x1;
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

