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
#include "spidriver.h"
#include "encoder.h"
#include "maxonmotor.h"
#include "fix16.h"


#define FIX16_1000  0x3E80000


/* Prototypes -------------------------------------------------------------- */
void initialize(void);
void initMemory(void);
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
shared_mem_t *s;

/* Param pointer */
param_mem_t *p;

/* Feedforward lookup table pointer */
lookUp_mem_t *l;

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

    /* Tare Encoder */
    if(s->cntrl_bit.encoderTare){
      encoderSetZeroAngle();
      s->cntrl_bit.encoderTare = 0;
    }

    /* Update State */
    updateState(cnt, buffIndx, stateIndx);

    debugPinLow();

    /* Poll for pru0 done bit (updating state)*/
    while(!(s->cntrl_bit.pru0_done));

    debugPinHigh();

    /* Reset pru0 done bit */
    s->cntrl_bit.pru0_done = 0;

    /* Update Control */
    updateControl(cnt, buffIndx, stateIndx);

    /* Set done bit (control update done) */
    s->cntrl_bit.pru1_done = 1;

    /* Increment state index */
    updateCounters(&cnt, &buffIndx, &stateIndx);

    /* Check enable bit */
    if(!(s->cntrl_bit.shdw_enable))
        break;

    /* Wait till interrupt is cleared */
    while(CT_INTC.SECR0 & (1 << 7));

    debugPinLow();
  }

  debugBuffer[8] = 0xBEAFDAAD;
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
  //imuInit();
  motorInit();


  // Zero some stuff
  p->stepRespCnt = 0;
  p->stepRespFlag = 0;
  p->stepCurrent = 0;

  p->FFgain = 0;
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
  fix16_t u_fb = 0; // (current of the motor)
  fix16_t u_ff = 0; // i_m (current of the motor)
  uint32_t t_cnts, temp;

  // Step Response
  if (s->cntrl_bit.stepResp == 1){

    if (!(p->stepRespFlag == 1)){
      p->stepRespCnt = cnt + 1000; // Delay 1 sec before step starts
      p->stepRespFlag = 1;
      u_ff = 0;
      u_fb = 0;
    }

    else if ( (p->stepRespFlag) & (cnt >= p->stepRespCnt) ){
      u_ff = p->stepCurrent;
      u_fb = 0;
    }
    else {
      u_ff = 0;
      u_fb = 0;
    }
  }

  // Feedforward Test
  else if (s->cntrl_bit.testFF == 1){

    // Calculate lut index t = (cnt % Tp) / Tp
    t_cnts = (cnt % 250) * 4;

    s->state[bi][si].l_percentGait = t_cnts;

    // Scale ff
    u_ff = fix16_smul(p->FFgain,
                        fix16_sdiv(fix16_from_int(l->u_ff[t_cnts]),
                                   FIX16_1000));
  }

  // Impedance and feedforward
  else{

    // Impedance
    //u_fb = fix16_smul(p->Kp, fix16_ssub(p->anklePos0, s->state[bi][si].anklePos));

    // Feedforward
    if (s->cntrl_bit.doFeedForward){
      // Calculate percent gait
      temp = (cnt - s->state[bi][si].l_hsStamp) * 1000;
      t_cnts = (uint32_t)
        fix16_to_int(fix16_sdiv(fix16_from_int((int32_t)temp),
                     fix16_from_int(s->state[bi][si].l_meanGaitPeriod)));

      // Saturate t_cnts;
      if (t_cnts >= NUM_FF_LT)
        t_cnts = NUM_FF_LT-1;

      // Store percent gait
      s->state[bi][si].l_percentGait = t_cnts;

      // Scale ff
      u_ff = fix16_smul(p->FFgain,
                        fix16_sdiv(fix16_from_int(l->u_ff[t_cnts]),
                                   FIX16_1000));
    }
  }

  // Send command/store command
  motorSetDuty(fix16_sadd(u_ff, u_fb), &s->state[bi][si].motorDuty);
  s->state[bi][si].u_ff = u_ff;
  s->state[bi][si].u_fb = u_fb;
}

void updateState(uint32_t cnt, uint8_t bi, uint8_t si)
{
  encoderSample(&(s->state[bi][si].anklePos));
  //imuSample(s->state[bi][si].imu);
}

void cleanUp(void)
{
  /* Add pru dependent peripheral cleanup methods here */
  encoderCleanUp();
  //imuCleanUp();
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
  s = (shared_mem_t *) ptr;

  /* Memory map for parameters (pru0 DRAM)*/
  ptr = (void *) PRU_OTHER_DRAM;
  p = (param_mem_t *) ptr;

  /* Memory map for feedforward lookup table (pru1 DRAM)*/
  ptr = (void *) PRU_DRAM;
  l = (lookUp_mem_t *) ptr;

  /* Point global debug buffer */
  debugBuffer = &(p->debugBuffer[0]);
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

