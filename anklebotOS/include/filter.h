#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

#include "mem_types.h"

void iirFixedInit(volatile int16_t *x, volatile int16_t *y, uint16_t bufLen);
int16_t iirFixedPoint(int16_t Q, int16_t N, int16_t *b, int16_t *a,
                      volatile int16_t *x, volatile int16_t *y, int16_t s);
#endif
