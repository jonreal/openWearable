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
#define DEBUG_R30_BIT     8

/* Prototypes -------------------------------------------------------------- */
static void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
static inline void interruptInit(void);
static inline void clearInterrupt(void);
static inline void pru1ToArmInterrupt(void);
static void pwmInit(void);
void disablePwm(void);
void setDuty(uint16_t duty);

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

  /* Local memory map for shared memory */
  ptr = NULL;
  ptr = (void *)PRU_L_SHARED_DRAM;
  p = (shared_mem_t *) ptr;

  /* Memory map for dram (params) */
  ptr = NULL;
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
    __R30 |= (1<<DEBUG_R30_BIT);

    /* Update Control */
    updateControl(cnt, buffIndx, stateIndx);

    __delay_cycles(1000);

    /* Increment state index */
    cnt++;
    stateIndx++;
    if(stateIndx == SIZE_OF_BUFFS){
      stateIndx = 0;
      buffIndx++;
      setDuty(1000);
      p->flow_control_bit.bufferFull = 1;
      if(buffIndx == NUM_OF_BUFFS){
        buffIndx = 0;
        setDuty(100);
      }
    }

    /* Pin Low */
    __R30 &= ~(1<<DEBUG_R30_BIT);

    /* Check for exit flag*/
    if(p->flow_control_bit.exit)
      break;
  }

  /* Pin Low */
  __R30 &= ~(1<<DEBUG_R30_BIT);

  /* Disable pwm */
  __delay_cycles(1000);
  setDuty(10000);
  __delay_cycles(1000);

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

static void updateControl(uint32_t cnt, uint8_t bi, uint8_t si)
{
  p->state[bi][si].motor_current_cmd = 0xdead;

  uint32_t *p32 = NULL;
  uint16_t *p16 = NULL;

  p16 = (uint16_t *) (EPWM2_BASE + 0x8);
  p->state[bi][si].timeStamp = *p16;

}


static void pwmInit(void)
{
  volatile uint32_t *p32 = NULL;
  volatile uint16_t *p16 = NULL;

  /* Disable Global enable interrupts */
  CT_INTC.GER = 0;

  /**** PWM SS 2 registers *****/

  /* IDVER: */
  p32 = (uint32_t *) (PWM_SS2_BASE);

  /* CLKCONFIG: set ePWMCLK_EN, everthing else 0 */
  p32 = (uint32_t *) (PWM_SS2_BASE + 0x8);
  *p32 = (0<<1) | (0<<5) | (1<<8);

  /**** EPWM registers *****/

  /* CMPCTL: shadow mode off */
  p16 = (uint16_t *) (EPWM2_BASE + 0xE);
  *p16 = (1<<4) | (1<<6);

  /* CMPA: compare reg */
  p16 = (uint16_t *) (EPWM2_BASE + 0x12);
  *p16 = 10000;

  /* CMPB: compare reg */
  p16 = (uint16_t *) (EPWM2_BASE + 0x14);
  *p16 = 0x0;

  /* AQCTLA: */
  p16 = (uint16_t *) (EPWM2_BASE + 0x16);
  *p16 = (0x2 << 4) | (0x1 << 6);

  /* AQCTLB: */
  p16 = (uint16_t *) (EPWM2_BASE + 0x18);
  *p16 = (0x2 << 4) | (0x1 << 6);

  /* TBCTL: CTRMODE = up-down-cnt mode (0x2) */
  p16 = (uint16_t *) (EPWM2_BASE);
  *p16 = 0x2;

  /* TBPRD: set period */
  p16 = (uint16_t *) (EPWM2_BASE + 0xA);
  *p16 = 10000;

  /*ETCLR: clear interrupts */
  p16 = (uint16_t *) (EPWM2_BASE + 0x38);
  *p16 = 0x1;

  /*ETSEL: enable pwm interrupt */
  p16 = (uint16_t *) (EPWM2_BASE + 0x32);
  *p16 = (0x1 << 3);

  /* Globaly enable interrupts */
  CT_INTC.GER = 1;

}

void setDuty(uint16_t duty)
{
  volatile uint16_t *p16 = NULL;

  /* CMPA: compare register */
  p16 = (uint16_t *) (EPWM2_BASE + 0x12);
  *p16 = duty;
}

void disablePwm(void)
{
  volatile uint16_t *p16 = NULL;

  /* CMPA: compare reg */
  p16 = (uint16_t *) (EPWM2_BASE + 0x12);
  *p16 = 10000;

}


static inline void interruptInit(void)
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

static inline void pru1ToArmInterrupt(void)
{
  __R31 = PRU1_ARM_INT;
}

static inline void clearInterrupt(void)
{
    __R31 = 0x00000000;
}

