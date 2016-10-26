#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

#include "spidriver.h"
#include "encoder.h"
#include "maxonmotor.h"
#include "fix16.h"

#include "pwmdriver.h"

#define FIX16_1000  0x3E80000
#define FIX16_MAX_ANGLE 0xFFE98000 // -22.5 (deg)

// Prototypes ----------------------------------------------------------------
void initialize(void);
void initMemory(void);
void updateState(uint32_t cnt, uint32_t si);
void updateControl(uint32_t cnt, uint32_t si);
void updateCounters(uint32_t *cnt, uint32_t *si);
void cleanUp(void);
void debugPinHigh(void);
void debugPinLow(void);

void interruptInit(void);
void clearInterrupt(void);

void calibratePWMcmp2current(uint32_t cnt, uint32_t si, uint16_t *cmpValue);

// Globals -------------------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

// State pointer
shared_mem_t *s;

// Param pointer
param_mem_t *p;

// Feedforward lookup table pointer
lookUp_mem_t *l;

// Debug Buffer
volatile uint32_t *debugBuffer;

// Main ----------------------------------------------------------------------
int main(void)
{
  uint32_t cnt = 0;
  uint32_t stateIndx = 0;
  uint16_t cmpValue = 0;

  initialize();

  // Uncomment, make, run, recomment to tare.
  //encoderSetZeroAngle();

  // Wait for host interrupt
  while( (__R31 & HOST1_MASK) == 0){}

  clearInterrupt();

  // Control Loop
  while(1){

    // Poll of IEP timer interrupt
    while((CT_INTC.SECR0 & (1 << 7)) == 0);

    debugPinHigh();

    // Update State
    updateState(cnt, stateIndx);

    debugPinLow();

    // Poll for pru0 done bit (updating state)
    while(!(s->cntrl_bit.pru0_done));

    debugPinHigh();

    // Reset pru0 done bit
    s->cntrl_bit.pru0_done = 0;

    // Update Control
    //calibratePWMcmp2current(cnt, stateIndx, &cmpValue);
    updateControl(cnt, stateIndx);

    // Set done bit (control update done)
    s->cntrl_bit.pru1_done = 1;

    // Increment state index
    updateCounters(&cnt, &stateIndx);

    // Check enable bit
    if(!(s->cntrl_bit.shdw_enable))
        break;

    // Wait till interrupt is cleared
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

  // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  // Pin Mux
  CT_CFG.GPCFG0 = 0;

  // Zero detect flags
  p->encoderDetect = 0;
  p->imuDetect = 0;

  // Memory and Interrupt
  initMemory();
  interruptInit();

  // Modules
  spiInit();

  // Add pru dependent peripheral init methods here
  if (encoderInit() == 0)
    p->encoderDetect = 1;
  else
    p->encoderDetect = 0;

  motorInit();

  //encoderSetZeroAngle();

  // Zero some stuff
  p->stepRespCnt = 0;
  p->stepRespFlag = 0;
  p->stepCurrent = 0;
  p->FFgain = 0;
}


void updateCounters(uint32_t* cnt, uint32_t* si)
{
  (*cnt)++;
  (*si)++;
  (*si) %= SIZE_STATE_BUFF;
}

void updateControl(uint32_t cnt, uint32_t si)
{
  fix16_t u_fb = 0;
  fix16_t u_ff = 0;
  uint32_t t_cnts, t1, Tp;

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

    s->state[si].l_percentGait = t_cnts;

    // Check to see if the ankle angle is at the hard limit
    if (s->state[si].anklePos < FIX16_MAX_ANGLE)
      u_ff = p->Kp;
    else
      // Scale ff
      u_ff = fix16_smul(p->FFgain,
                fix16_sdiv(fix16_from_int(l->u_ff[t_cnts]), FIX16_1000));
  }

  // Impedance and feedforward
  else{

    // Impedance feedback
    //u_fb = fix16_smul(p->Kp, fix16_ssub(p->anklePos0, s->state[si].anklePos));

    // Constant current offset
    u_fb = p->Kp;


    // Feedforward
    if ((s->cntrl_bit.doFeedForward) && (p->gaitDetectReady)){

      // Time since hs
      t1 = (cnt - s->state[si].l_hsStamp) * 1000;

      t_cnts = t1 / (s->state[si].l_meanGaitPeriod -
                     s->state[si].l_meanGaitPeriod / 1000);

      // Saturate t_cnts;
      if (t_cnts >= NUM_FF_LT)
        t_cnts = NUM_FF_LT-1;

      // Store percent gait
      s->state[si].l_percentGait = t_cnts;


      // Check to see if the ankle angle is at the hard limit
      if (s->state[si].anklePos < FIX16_MAX_ANGLE)
        u_ff = 0;
      else
        // Scale ff
        u_ff = fix16_smul(p->FFgain,
                  fix16_sdiv(fix16_from_int(l->u_ff[t_cnts]), FIX16_1000));
    }
  }

  // Send command/store command
  motorSetCurrent(fix16_sadd(u_ff,u_fb), &s->state[si].motorCmpValue);
  s->state[si].u_ff = u_ff;
  s->state[si].u_fb = u_fb;
}

void updateState(uint32_t cnt, uint32_t si)
{
  if (p->encoderDetect)
    encoderSample(&(s->state[si].anklePos));
  else
    s->state[si].anklePos = 0;
}

void cleanUp(void)
{
  // Add pru dependent peripheral cleanup methods here
  encoderCleanUp();
  motorCleanUp();

  // Cleanup modules
  spiCleanUp();

  // Clear all interrupts
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

  // Memory map for shared memory
  ptr = (void *)PRU_L_SHARED_DRAM;
  s = (shared_mem_t *) ptr;

  // Memory map for parameters (pru0 DRAM)
  ptr = (void *) PRU_OTHER_DRAM;
  p = (param_mem_t *) ptr;

  // Memory map for feedforward lookup table (pru1 DRAM)
  ptr = (void *) PRU_DRAM;
  l = (lookUp_mem_t *) ptr;

  // Point global debug buffer
  debugBuffer = &(p->debugBuffer[0]);
}

void interruptInit(void)
{
  // Disable Global enable interrupts
  CT_INTC.GER = 0;

  // Clear any pending PRU-generated events
  __R31 = 0x00000000;

  // Clear the status of all interrupts
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  // Gloablly enable interrupts
  CT_INTC.GER = 1;
}

void clearInterrupt(void)
{
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
   __R31 = 0x00000000;
}


void calibratePWMcmp2current(uint32_t cnt, uint32_t si, uint16_t *cmpValue)
{
  // Wait 10s before starting cmpValue sweep
  if (cnt > 10000){

    // Increase cmpValues every sec.
    if ((cnt % 1000) == 0){
      (*cmpValue) = (*cmpValue) + 100;
      if (*cmpValue >= 10000)
        (*cmpValue) = 0;
    }

    pwmSetCmpValue(*cmpValue);
    s->state[si].motorCmpValue = fix16_from_int(*cmpValue);

    uint16_t val = 10000;
    s->state[si].motorCmpValue = fix16_from_int(val);
    pwmSetCmpValue(val);
  }
  else
    s->state[si].motorCmpValue = 0;


}

