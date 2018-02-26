#ifndef _EMG_H_
#define _EMG_H_

#include "fix16.h"

// Vadc/ADC_resolution (1800 mV / 4095 bits)
#define FIX16_BITS2VOLTS (0x708A)

//---- Global ----
extern volatile uint32_t *debug_buff;

//---- Prototypes ----
void emgInit(void);
void emgCleanup(void);
fix16_t emgSampleBicep(void);

#endif
