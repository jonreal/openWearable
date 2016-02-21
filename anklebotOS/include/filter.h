#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

#include "mem_types.h"

/* --- Global Debug Buffer --------------------------------------------------*/
extern volatile uint32_t *debugBuffer;

/* --- Typedef --------------------------------------------------------------*/ 

/* --- Prototypes -----------------------------------------------------------*/
void fix16_iirInit(volatile fix16_t *x, volatile fix16_t *y, uint32_t len);
fix16_t fix16_iir(uint32_t N, fix16_t *b, fix16_t *a,
                  volatile fix16_t *x, volatile fix16_t *y, int16_t in);
#endif
