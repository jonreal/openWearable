#ifndef _EMG_H_
#define _EMG_H_

#include "fix16.h"

typedef struct {
  fix16_t bits;
} emg_state_t;

typedef struct {
  uint8_t adc_ch;
  volatile emg_state_t s;
} emg_t;

emg_t* EmgInitSensor(uint8_t chan);
void EmgCleanup(emg_t* emg);
void EmgUpdate(emg_t* emg);
fix16_t EmgGetBits(emg_t* emg);

#endif
