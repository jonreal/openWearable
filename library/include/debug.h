/* Copyright 2019 Jonathan Realmuto

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

#ifndef _DEBUG_H__
#define _DEBUG_H_

// "P9.13", gpio0[31]
#define CPU_DEBUG_CHIP 0
#define CPU_DEBUG_PIN 31

int DebugInit(void);
void DebugPinHigh(void);
void DebugPinLow(void);
void DebugCleanup(void);

#endif
