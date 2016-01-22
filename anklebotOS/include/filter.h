#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

#define FILTER_LEN 51


void firFixedInit(volatile int16_t *insamp);
int16_t firFixed(int16_t *coeffs, volatile int16_t *insamp, int16_t input);

#endif
