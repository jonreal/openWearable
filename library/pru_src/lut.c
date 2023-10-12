/* Copyright 2023 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include "lut.h"

// Assume LUT is length 1000, and has been scaled by 1000
fix16_t LutLerp(const int16_t* lut, fix16_t x) {
 int32_t i0 = fix16_to_int(x);
 int32_t i1 = i0 + 1;
 fix16_t x0 = fix16_from_int(i0);
 fix16_t x1 = fix16_from_int(i1);

 if (i0 <= 0)
   return fix16_sdiv(fix16_from_int((int32_t)lut[0]),FIX16_100);
 else if (i1 >= FIX16_999)
   return fix16_sdiv(fix16_from_int((int32_t)lut[999]),FIX16_100);

 fix16_t y0 = fix16_sdiv(fix16_from_int((int32_t)lut[i0]),FIX16_100);
 fix16_t y1 = fix16_sdiv(fix16_from_int((int32_t)lut[i1]),FIX16_100);

 return fix16_smul(y0,fix16_ssub(x1,x)) + fix16_smul(y1,fix16_ssub(x,x0));
}
