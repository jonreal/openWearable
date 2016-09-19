#include <stdlib.h>
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_iep.h>
#include <pru_intc.h>

#include "rsc_table.h"
#include "mem_types.h"
#include "hw_types.h"

#include "adcdriver.h"
#include "viconsync.h"
#include "filter.h"


// Prototypes ----------------------------------------------------------------
void initialize(void);
void initMemory(void);
void updateState(uint32_t cnt, uint32_t si);
void updateControl(uint32_t cnt, uint32_t si);
void updateCounters(uint32_t *cnt, uint32_t *si);
void cleanUp(void);
void debugPinHigh(void);
void debugPinLow(void);

void iepTimerInit(uint32_t count);
void iepInterruptInit(void);
void startTimer(void);
void clearTimerFlag(void);
void clearIepInterrupt(void);
void pru0ToArmInterrupt(void);

// Globals -------------------------------------------------------------------
volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Constant Table
volatile far pruIntc CT_INTC
  __attribute__((cregister("PRU_INTC", far), peripheral));

volatile pruCfg CT_CFG
  __attribute__((cregister("PRU_CFG", near), peripheral));

volatile far pruIep CT_IEP
  __attribute__((cregister("PRU_IEP", near), peripheral));

// Shared memory pointer
shared_mem_t *s;

// Param pointer
param_mem_t *p;

// LookUp tables
lookUp_mem_t *l;

// Debug Buffer
volatile uint32_t *debugBuffer;

// Main ----------------------------------------------------------------------
int main(void)
{
  uint32_t cnt = 0;
  uint32_t stateIndx = 0;

  initialize();

  // Wait for host (ARM) interrupt
  while( (__R31 & HOST0_MASK) == 0){}

  clearIepInterrupt();
  startTimer();

  // Control Loop
  while(1){

    // Poll for IEP timer interrupt
    while((CT_INTC.SECR0 & (1 << 7)) == 0){}

    clearTimerFlag();
    debugPinHigh();

    // Shadow enable bit
    s->cntrl_bit.shdw_enable = s->cntrl_bit.enable;

    // Update State
    updateState(cnt, stateIndx);

    // Set done bit (update state done)
    s->cntrl_bit.pru0_done = 1;

    // Poll for pru1 to be done (control update done)
    while(!(s->cntrl_bit.pru1_done));

    // Reset done bit
    s->cntrl_bit.pru1_done = 0;

    // Increment state index
    updateCounters(&cnt, &stateIndx);

    // Check enable bit
    if(!(s->cntrl_bit.shdw_enable))
        break;

    clearIepInterrupt();
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

  /* PRU CTRL: CTR_EN = 0x1 - enable cycle counter */
  HWREG(PRU_CTRL_BASE) |= (1 << 3);

  /* Pin Mux */
  CT_CFG.GPCFG0 = 0;

  /*** Memory ***/
  initMemory();

  /* Init timer */
  iepInterruptInit();
  iepTimerInit(p->frq_clock_ticks);
  clearIepInterrupt();

  /* Add pru dependent peripheral init methods here */
  adcInit();

  /* Init filter buffers */
  fix16_iirInit(p->filtBuffer[0].x, p->filtBuffer[0].y, MAX_IIR_ORDER+1);
  fix16_iirInit(p->filtBuffer[1].x, p->filtBuffer[1].y, MAX_IIR_ORDER+1);
  fix16_iirInit(p->filtBuffer[2].x, p->filtBuffer[2].y, MAX_IIR_ORDER+1);
  fix16_iirInit(p->filtBuffer[3].x, p->filtBuffer[3].y, MAX_IIR_ORDER+1);
  fix16_iirInit(p->filtBuffer[4].x, p->filtBuffer[4].y, MAX_IIR_ORDER+1);
  fix16_iirInit(p->filtBuffer[5].x, p->filtBuffer[5].y, MAX_IIR_ORDER+1);
}

void initMemory(void)
{
  void *ptr = NULL;

  // Memory map for shared memory
  ptr = (void *)PRU_L_SHARED_DRAM;
  s = (shared_mem_t *) ptr;

  // Memory map for parameters (pru0 DRAM)
  ptr = (void *) PRU_DRAM;
  p = (param_mem_t *) ptr;

  // Memory map for feedforward lookup table (pru1 DRAM)
  ptr = (void *) PRU_OTHER_DRAM;
  l = (lookUp_mem_t *) ptr;

  // Point global debug buffer
  debugBuffer = &(p->debugBuffer[0]);
}

void updateState(uint32_t cnt, uint32_t si)
{
  int16_t adc[8];
  fix16_t s1, s2, s3, s4, s5, s6;

  s->state[si].timeStamp = cnt;
  s->state[si].sync = viconSync();

  // Insole samples
  adcSample_2(adc); // adc[2,3,4]
  adcSample_3(adc); // adc[5,6,7]

  // Filter insoles heel
  s1 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[0].x, p->filtBuffer[0].y,
                 adc[2]);

  s2 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[1].x, p->filtBuffer[1].y,
                 adc[3]);

  s3 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[2].x, p->filtBuffer[2].y,
                 adc[4]);

  s4 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[3].x, p->filtBuffer[3].y,
                 adc[5]);

  s5 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[4].x, p->filtBuffer[4].y,
                 adc[6]);

  s6 = fix16_iir(p->filt.N, p->filt.b, p->filt.a,
                 p->filtBuffer[5].x, p->filtBuffer[5].y,
                 adc[7]);

  // Pack Stuct
  s->state[si].adc[2] = (int16_t) fix16_to_int(s1);  // toe
  s->state[si].adc[3] = (int16_t) fix16_to_int(s2);  // .
  s->state[si].adc[4] = (int16_t) fix16_to_int(s3);  // .
  s->state[si].adc[7] = (int16_t) fix16_to_int(s4);  // .
  s->state[si].adc[6] = (int16_t) fix16_to_int(s5);  // .
  s->state[si].adc[5] = (int16_t) fix16_to_int(s6);  // heal
}

void updateControl(uint32_t cnt, uint32_t si)
{
}

void updateCounters(uint32_t *cnt, uint32_t *si)
{
  // Set buffer location for circ buffer
  s->stateIndex = (*si);

  (*cnt)++;
  (*si)++;
  (*si) %= SIZE_STATE_BUFF;
}

void cleanUp(void)
{
  // Add pru dependent peripheral cleanup methods here
  adcCleanUp();

  // Clear all interrupts
  clearIepInterrupt();
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;
}

void debugPinHigh(void)
{
  __R30 |= (1 << PRU0_DEBUG_PIN);
}

void debugPinLow(void)
{
  __R30 &= ~(1 << PRU0_DEBUG_PIN);
}

void iepTimerInit(uint32_t count)
{
  // Enable ocp_clk
  CT_CFG.IEPCLK_bit.OCP_EN = 1;

  // Disable counter
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  // Reset Count register
  CT_IEP.TMR_CNT = 0x0;

  // Clear overflow status register
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

  // Set compare value
  CT_IEP.TMR_CMP0 = (count-1);

  // Clear compare status
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  // Disable compensation
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

  // Enable CMP0 and reset on event
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;
}

void iepInterruptInit(void)
{
  /*** System event 7 Interrupt -> Host 1 ***/

  // Disable Global enable interrupts
  CT_INTC.GER = 0;

  // Clear any pending PRU-generated events
  __R31 = 0x00000000;

  // Map event 7 to channel 2
  CT_INTC.CMR1_bit.CH_MAP_7 = 1;

  // Map Channel 2 to host 2
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1;

  // Clear the status of all interrupts
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  // Enable event 7
  CT_INTC.EISR = 7;

  // Enable Host interrupt 1
  CT_INTC.HIEISR |= (1 << 0);

  // Gloablly enable interrupts
  CT_INTC.GER = 1;
}

void startTimer(void)
{
  // Start Timer
  CT_IEP.TMR_GLB_CFG = 0x11;
}

void clearTimerFlag(void)
{
  // Clear compare status
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;
}

void pru0ToArmInterrupt(void)
{
  __R31 = PRU0_ARM_INT;
}

void clearIepInterrupt(void)
{
    // Clear interrupt status
    CT_INTC.SECR0 = (1<<7);
    __R31 = 0x00000000;
}

