#ifndef _EMG_H_
#define _EMG_H_

#include "fix16.h"



//---- Global ----
extern volatile uint32_t *debug_buff;

//---- Prototypes ----
void EmgInit(void);
void EmgCleanup(void);
fix16_t EmgSampleBicep(void);

#endif
