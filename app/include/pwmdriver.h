#ifndef _PWMDRIVER_H_
#define _PWMDRIVER_H_

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototype ---- */
void pwmInit(void);
void pwmCleanUp(void);
void pwmSetCmpValue(uint16_t cmpvalue);

#endif
