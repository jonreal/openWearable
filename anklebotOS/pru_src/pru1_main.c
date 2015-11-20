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
#include "maxonmotor.h"
#include "interp.h"

/* Local Params (mirror) --------------------------------------------------- */
typedef struct{
  volatile uint16_t Kp;
  volatile uint16_t Kd;
  volatile int16_t anklePos0;
}local_t;

uint32_t tt = 0;


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

void interruptInit(void);
void clearInterrupt(void);

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
//ff_mem_t *ff;
lookUp_mem_t *lookUp;

/* Local params */
local_t loc;

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

    if(p->cntrl_bit.encoderTare){
      encoderSetZeroAngle();
      p->cntrl_bit.encoderTare = 0;
    }

    /* Update (mirror) params */
    updateLocalParams();

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
  cleanUp();
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

  interruptInit();

  /* Add pru dependent peripheral init methods here */
  encoderInit();
  motorInit();
}

void updateLocalParams(void)
{
  loc.Kp = param->Kp;
  loc.Kd = param->Kd;
  loc.anklePos0 = param->anklePos0;
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
  int16_t u_fb = 0;
  int16_t u_ff = 0;

  uint32_t Tp_cnts = 1000;
  int t_cnts = 0;

  /* Impedance Feedback */
  //cmd = ((int16_t)loc.Kp)*(loc.anklePos0 - p->state[bi][si].anklePos)/100;
  //motorSetDuty(cmd, &p->state[bi][si].motorDuty);
  //

  /* Feedforward */
  if(p->cntrl_bit.doFeedForward)
  {
//    t_cnts = (((cnt % Tp_cnts)*1000)/Tp_cnts);
    p->state[bi][si].ankleVel = t_cnts;
    u_ff = lookUp->ff_ankleTorque[(tt%512)];
    tt++;
  }
  p->state[bi][si].motorDuty = u_ff;

//  motorSetDuty(u_ff + u_fb, &p->state[bi][si].motorDuty);
}

void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  encoderSample(&p->state[bi][si].anklePos);
}

void cleanUp(void)
{
  /* Add pru dependent peripheral cleanup methods here */
  encoderCleanUp();
  motorCleanUp();

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
  lookUp = (lookUp_mem_t *) ptr;

  /* Point global debug buffer */
  debugBuffer = &(param->debugBuffer[0]);
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

void clearInterrupt(void)
{
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
   __R31 = 0x00000000;
}

