#include <stdint.h>

#include "viconsync.h"

/* Sync pin goes high */
uint16_t viconSync(void)
{
  if( (__R31 & (1 << SYNC_PIN)) == (1 << SYNC_PIN)){
    return 1;
  }
  return 0;
}
