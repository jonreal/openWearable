#ifndef _EMG_H_
#define _EMG_H_

#include "fix16.h"

typedef struct {
  uint8_t adc_ch; 
} emg_t;

//---- Global ----
extern volatile uint32_t *debug_buff;

//---- Prototypes ----
emg_t* EmgInitSensor(uint8_t chan);
void EmgCleanup(emg_t* sens);
fix16_t EmgSample(const emg_t* sens);

#endif
