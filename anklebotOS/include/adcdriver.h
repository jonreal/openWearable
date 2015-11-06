#ifndef _ADCDRIVER_H_
#define _ADCDRIVER_H_

#define MUX_SEL_PIN     7

/* --- Digital output reg. */
volatile register uint32_t __R30;

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void adcInit(void);
void adcSample(volatile uint16_t adc[8]);
void adcCleanUp(void);

#endif
