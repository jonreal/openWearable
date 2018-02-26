/* Copyright 2017 Jonathan Realmuto

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

#ifndef _TUI_H__
#define _TUI_H_

#include "pru.h"

int TuiInit(void);
void TuiInputCallback(int sig);
void TuiPrintMenu(void);
int TuiLoop(const pru_mem_t* pru_mem);
int TuiCleanup(void);

#endif /* _TUI_H_ */
