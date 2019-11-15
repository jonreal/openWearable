#ifndef _EMG_H_
#define _EMG_H_

#include "fix16.h"
#include "filter.h"

typedef struct {
  fix16_t raw;
  fix16_t value;
} emg_state_t;

typedef struct {
  uint8_t adc_ch;
  volatile emg_state_t s;
  iir_filt_t* hp;
  iir_filt_t* lp;
} emg_t;

emg_t* EmgInitSensor(uint8_t chan, iir_filt_t* hp, iir_filt_t* lp);
void EmgCleanup(emg_t* emg);
void EmgUpdate(emg_t* emg);
emg_state_t EmgGetState(emg_t* emg);

#endif
