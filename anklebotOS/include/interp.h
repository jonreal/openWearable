#ifndef _INTERP_H_
#define _INTERP_H_

/* Global ------------------------------------------------------------------ */
extern volatile uint32_t *debugBuffer;

/* Prototypes -------------------------------------------------------------- */
int16_t interp(volatile int16_t *tab, int16_t t);

#endif

