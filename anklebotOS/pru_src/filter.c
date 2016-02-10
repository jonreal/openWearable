#include <stdint.h>
#include <stdio.h>

#include "filter.h"


// ----------------------------------------------------------------------------

/* IIR init */
void iirFixedInit(volatile int16_t *x, volatile int16_t *y, uint16_t bufLen)
{
  for(int i=0; i<bufLen; i++){
    x[i] = 0;
    y[i] = 0;
  }
}

// ----------------------------------------------------------------------------

// Fixed point iir filter
int16_t iirFixedPoint(int16_t Q, int16_t N, int32_t *b, int32_t *a,
                      volatile int16_t *x, volatile int16_t *y, int16_t s)
{
  int64_t acc;     // accumulator for MACs
  int16_t output;

  // Shift samples back in time
  for(int i=N; i>0; i--){
    x[i] = x[i-1];
    y[i] = y[i-1];
  }

  // new Sample
  x[0] = s;

  // load rounding constant (one less than final bits e.g. int16_t = (15 -1))
//  acc = 1 << 14;

  // difference eq.
  acc = (int64_t)b[0] * (int64_t)x[0];

  for(int k=1; k<N+1; k++) {

    acc += (int64_t)b[k] * (int64_t)x[k] - (int64_t)a[k] * (int64_t)y[k];

    // saturate the result
    if ( acc > 0x3fffffffffffffff) {
      acc = 0x3fffffffffffffff;
    }
    else if ( acc < -0x4000000000000000) {
      acc = -0x4000000000000000;
    }
  }

  // Rounding
  int64_t ahalf = acc >= 0 ? (1<<((Q/2)-1)) : -(1<<((Q/2)-1));

  // Apply scaling and cast as interger
  output = (int16_t)( (acc+ahalf) / (1<<Q));

  y[0] = output;
  return output;
}

///* FIR init */
//void firFixedInit(volatile int16_t *insamp)
//{
//  for(int i=0; i < FILTER_LEN; i++){
//    insamp[i] = 0;
//  }
//}
///* FIR filter function */
//int16_t firFixed(int16_t *coeffs, volatile int16_t *insamp, int16_t input)
//{
//    int32_t acc;     // accumulator for MACs
//    int16_t output;
//
//    // shift input samples back in time
//
//    for(int i=FILTER_LEN; i > 0; i--){
//      insamp[i] = insamp[i-1];
//    }
//
//    // put the new samples at the high end of the buffer
//    insamp[0] = input;
//
//    // load rounding constant
//    acc = 1 << 14;
//
//    // perform the multiply-accumulate
//    for(int k = 0; k < FILTER_LEN; k++ ) {
//      acc += (int32_t)coeffs[k] * (int32_t)insamp[k];
//    }
//    // saturate the result
//    if ( acc > 0x3fffffff ) {
//      acc = 0x3fffffff;
//    } else if ( acc < -0x40000000 ) {
//      acc = -0x40000000;
//    }
//    // convert from Q30 to Q15
//    output = (int16_t)(acc >> 15);
//
//  return output;
//}
