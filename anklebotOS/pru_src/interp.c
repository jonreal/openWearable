#include <stdint.h>

#include "interp.h"
#include "mem_types.h"

int16_t interp(volatile int16_t *tab, uint32_t t)
{
  int16_t diff_t;

  for(int i=0; i<NUM_FF_LT-1; i++){
    if(t >= i && t <= i+1)
    {
      diff_t = t - i;
      return tab[i] + (tab[i+1] - tab[i])*diff_t;
    }
  }
}
