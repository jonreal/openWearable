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

#ifndef _UDF_H_
#define _UDF_H_

#include "mem_types.h"

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debugBuffer;
extern param_mem_t *p;
extern shared_mem_t *s;
extern lookUp_mem_t *l;

// Prototypes ---------------------------------------------------------------
void init_pru0(void);
void init_pru1(void);
void updateState_pru0(uint32_t cnt, uint32_t si);
void updateState_pru1(uint32_t cnt, uint32_t si);
void updateControl_pru0(uint32_t cnt, uint32_t si);
void updateControl_pru1(uint32_t cnt, uint32_t si);
void cleanup_pru0(void);
void cleanup_pru1(void);

#endif
