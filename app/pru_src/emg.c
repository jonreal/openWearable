#include <stdint.h>

#include "emg.h"
#include "adcdriver.h"

void EmgInit(void) {
  for (int i=0; i<8; i++) {
    adcSampleCh(i);
  }
}

void EmgCleanup(void) {
}

fix16_t EmgSampleBicep(void) {
  return fix16_smul(fix16_from_int((int)adcSampleCh(0)),FIX16_BITS2VOLTS);
}
