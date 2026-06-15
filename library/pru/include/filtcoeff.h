#ifndef _FILTCOEFF_H_
#define _FILTCOEFF_H_

#include "fix16.h"

// Fix16 filter coefficients


// Butterworth IIR filters
//
// 1st order lowpass 2Hz
static const fix16_t k_lp_1_2Hz_b[2] = {0x196, 0x196};
static const fix16_t k_lp_1_2Hz_a[2] = {fix16_one, 0xFFFF0333};

// 1st order lowpass 3Hz
static const fix16_t k_lp_1_3Hz_b[2] = {0x261, 0x261};
static const fix16_t k_lp_1_3Hz_a[2] = {fix16_one, 0xFFFF04CA};

// 1st order lowpass 5Hz
static const fix16_t k_lp_1_5Hz_b[2] = {0x3F8, 0x3F8};
static const fix16_t k_lp_1_5Hz_a[2] = {fix16_one, 0xFFFF07E9};

// 1st order lowpass 10Hz
static const fix16_t k_lp_1_10Hz_b[2] = {0x7CF, 0x7CF};
static const fix16_t k_lp_1_10Hz_a[2] = {fix16_one, 0xFFFF0F97};

// 1st order lowpass 30Hz
static const fix16_t k_lp_1_30Hz_b[2] = {0x161E, 0x161E};
static const fix16_t k_lp_1_30Hz_a[2] = {fix16_one, 0xFFFF2C36};

// 1st order lowpass 300Hz
static const fix16_t k_lp_1_100Hz_b[2] = {0x3EC5, 0x3EC5};
static const fix16_t k_lp_1_100Hz_a[2] = {fix16_one, 0xFFFF7D91};


#endif
