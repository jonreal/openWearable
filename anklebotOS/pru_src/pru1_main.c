#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

#include "fix16.h"
#include "i2cdriver.h"
#include "i2cBusPCA9548A.h"
#include "fyberHaptic.h"


#define FIX16_1000  0x3E80000


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

void initSlope(void);
void setAllAmps(uint32_t cnt, uint32_t si);
void mode1(uint32_t si);
void mode0(uint32_t si);
int16_t force2Haptic(int16_t force);

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

  initialize();

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

  // Memory and Interrupt
  initMemory();
  interruptInit();

  // Modules
  i2cInit();

  // Periph.
  hapticInit();

  initSlope();
}


void updateCounters(uint32_t* cnt, uint32_t* si)
{
  (*cnt)++;
  (*si)++;
  (*si) %= SIZE_STATE_BUFF;
}

void updateControl(uint32_t cnt, uint32_t si)
{
  if (p->hapticMode == 1)
    mode1(si);
  else
    mode0(si);

  setAllAmps(cnt, si);
}

void updateState(uint32_t cnt, uint32_t si)
{

}

void cleanUp(void)
{
  // Add pru dependent peripheral cleanup methods here
  hapticCleanUp();
  i2cBusCleanUp();

  // Cleanup modules
  i2cCleanUp();

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


/// Haptic
void setAllAmps(uint32_t cnt, uint32_t si)
{
  for (int i=1; i<7; i++){
    setAmplitude(i, cnt, s->state[si].hapticAmp[i-1]);
  }
}

void mode1(uint32_t si)
{
  for (int i=1; i<7; i++){
    if (s->state[si].adc[i+1] >= p->threshold)
      s->state[si].hapticAmp[i-1] = force2Haptic(s->state[si].adc[i-1]);
    else
      s->state[si].hapticAmp[i-1] = 0;
  }
}

void mode0(uint32_t si)
{
  for (int i=1; i<7; i++)
    s->state[si].hapticAmp[i-1] = 0;
}

int16_t force2Haptic(int16_t force)
{
  return (int16_t) fix16_to_int(
          fix16_sadd(fix16_smul(p->slope, fix16_from_int(force)), p->intercept));
}


void initSlope(void)
{
  fix16_t num = fix16_from_int(p->hapticMaxAmp);
  fix16_t den = fix16_ssub(fix16_from_int(p->saturation),
                           fix16_from_int(p->threshold));
  p->slope = fix16_sdiv(num,den);
  fix16_t negSlope = fix16_smul(p->slope,fix16_from_int(-1));
  p->intercept = fix16_smul(negSlope,fix16_from_int(p->threshold));
}


