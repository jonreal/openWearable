#include <stdint.h>

#include "viconsync.h"

/* Sync pin goes high */
int viconSync(void)
{
  if( (__R31 & (1 << SYNC_PIN)) == 1){
    return 1;
  }
  return 0;
}
