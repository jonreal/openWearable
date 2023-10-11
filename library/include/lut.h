#ifndef _LUT_H_
#define _LUT_H_

#include "fix16.h"

#define FIX16_1000 (0x3E80000)
#define FIX16_999 (0x3E70000)
#define FIX16_05 (0x8000)

int32_t LutLerp(const int16_t* lut, fix16_t x);


#endif
