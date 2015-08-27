#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"

/* Prototypes -------------------------------------------------------------- */
static void updateControl(uint32_t cnt, uint8_t bi, uint8_t si);
static inline void interruptInit(void);
static inline void clearInterrupt(void);
static inline void pru1ToArmInterrupt(void);

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
  uint16_t stateIndx = 0;
  unsigned buffIndx = 0;

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

  interruptInit();

  while( (__R31 & HOST1_MASK) == 0){}

  clearInterrupt();

  /*** Control Loop ***/
  while(1){

    /* Check for pru0 sensor done bit */
    if(p->flow_control_bit.sensorDone){

      /* Reset sensor bit */
      p->flow_control_bit.sensorDone = 0;

      /* Get buffer head */
      buffIndx = p->flow_control_bit.writeBuffer;
      stateIndx = p->flow_control_bit.stateIndx;

      /* Pin High */
      __R30 |= (1<<0);

      /* Update Control */
      updateControl(cnt, buffIndx, stateIndx);

      /* Computations */
      __delay_cycles(1000);

      /* if buffer full alert ARM */
      if(stateIndx == (SIZE_OF_BUFFS-1))
        p->flow_control_bit.bufferFull = 1;


      /* Pin Low */
      __R30 &= ~(1<<0);

      /* Check for exit flag*/
      if(p->flow_control_bit.exit)
        break;

      cnt++;
    }

  }

  /* Pin Low */
  __R30 &= ~(1<<0);

  /* Clear all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* Send interrupt to ARM */
  pru1ToArmInterrupt();

  __halt();
  return 0;
}

static void updateControl(uint32_t cnt, uint8_t bi, uint8_t si)
{
  p->state[bi][si].gaitPhase = cnt;
  p->state[bi][si].motor_current_cmd = cnt;
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

