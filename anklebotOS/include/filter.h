#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

#define FILTER_LEN 51


void firFixedInit(volatile int16_t *insamp);
int16_t firFixed(int16_t *coeffs, volatile int16_t *insamp, int16_t input);
int16_t iirFixedPoint(int16_t N, int16_t *b, int16_t *a,
                      int16_t *x, int16_t *y, int16_t s);
#endif
