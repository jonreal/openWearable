#ifndef _FILTCOEFF_H_
#define _FILTCOEFF_H_

#include "fix16.h"

// Fix16 filter coefficients


// Butterworth IIR filters

static const fix16_t k_lp_1_5Hz_b[2] = {0x3F8, 0x3F8};
static const fix16_t k_lp_1_5Hz_a[2] = {0x10000, 0xFFFF07E9};

#endif
