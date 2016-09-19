#include <stdint.h>
#include <stdio.h>

#include "fix16.h"
#include "filter.h"


// ----------------------------------------------------------------------------
// Initialize filter buffers
void fix16_iirInit(volatile fix16_t *x, volatile fix16_t *y, uint32_t len)
{
  for(int i=0; i<len; i++){
    x[i] = 0;
    y[i] = 0;
  }
}

// ----------------------------------------------------------------------------
// Fixed point Q16.16 (really Q15.16) iir filter
fix16_t fix16_iir(uint32_t N, fix16_t *b, fix16_t *a,
                  volatile fix16_t *x, volatile fix16_t *y, int16_t in)
{
  fix16_t bx, ay, out;

  // Shift samples back in time
  for(int i=N; i>0; i--){
    x[i] = x[i-1];
    y[i] = y[i-1];
  }

  // new Sample
  x[0] = fix16_from_int((int32_t)in);

  // difference eq.
  out = fix16_smul(b[0], x[0]);
  for(int k=1; k<N+1; k++) {
    bx = fix16_smul(b[k], x[k]);
    ay = fix16_smul(a[k], y[k]);
    out = fix16_sadd(out, fix16_ssub(bx, ay));
  }
  y[0] = out;
  return out;
}
