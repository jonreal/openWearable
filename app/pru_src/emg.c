#include <stdint.h>

#include "emg.h"
#include "adcdriver.h"

void emgInit(void)
{
  for(int i=0; i<8; i++){
    adcSampleCh(i);
  }

}

void emgCleanup(void)
{
}

// emg in mV
fix16_t emgSampleBicep(void)
{
  return fix16_smul(fix16_from_int((int)adcSampleCh(0)),FIX16_BITS2VOLTS);
}
