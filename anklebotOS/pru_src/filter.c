#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "filter.h"


// ----------------------------------------------------------------------------

/* FIR init */
void firFixedInit(volatile int16_t *insamp)
{
  for(int i=0; i < FILTER_LEN; i++){
    insamp[i] = 0;
  }
}

// ----------------------------------------------------------------------------

// Fixed point iir filter
int16_t iirFixedPoint(int16_t N, int16_t *b, int16_t *a,
                      int16_t *y, int16_t *x, int16_t s)
{
  int32_t acc;     // accumulator for MACs
  int16_t output;

  // new Sample
  x[0] = s;

  // load rounding constant
  acc = 1 << 14;

  // difference eq.
  acc = (int32_t) b[0] * x[0];
  for(int k=1; k<N+1; k++) {
    acc += (int32_t)b[k] * (int32_t)x[k] - (int32_t)a[k] * (int32_t)y[k];
  }

  // saturate the result
  if ( acc > 0x3fffffff ) {
    acc = 0x3fffffff;
  } else if ( acc < -0x40000000 ) {
    acc = -0x40000000;
  }

  // convert from Q30 to Q15
  output = (int16_t)(acc >> 15);

  // Shift samples back in time
  for(int i=N+1; i>0; i--){
    x[i] = x[i-1];
    y[i] = y[i-1];
  }
  return output;
}


/* FIR filter function */
int16_t firFixed(int16_t *coeffs, volatile int16_t *insamp, int16_t input)
{
    int32_t acc;     // accumulator for MACs
    int16_t output;

    // shift input samples back in time

    for(int i=FILTER_LEN; i > 0; i--){
      insamp[i] = insamp[i-1];
    }

    // put the new samples at the high end of the buffer
    insamp[0] = input;

    // load rounding constant
    acc = 1 << 14;

    // perform the multiply-accumulate
    for(int k = 0; k < FILTER_LEN; k++ ) {
      acc += (int32_t)coeffs[k] * (int32_t)insamp[k];
    }
    // saturate the result
    if ( acc > 0x3fffffff ) {
      acc = 0x3fffffff;
    } else if ( acc < -0x40000000 ) {
      acc = -0x40000000;
    }
    // convert from Q30 to Q15
    output = (int16_t)(acc >> 15);

  return output;
}
