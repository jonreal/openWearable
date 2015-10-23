#ifndef _ADCDRIVER_H_
#define _ADCDRIVER_H_

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void adcInit(void);
void adcSample(volatile uint16_t adc[8]);
void adcCleanUp(void);

#endif
