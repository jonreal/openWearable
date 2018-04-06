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
extern volatile uint32_t* debug_buff;

// Prototypes ---------------------------------------------------------------
void Pru0Init(void);
void Pru0UpdateState(const pru_count_t* c, pru_mem_t* mem);
void Pru0UpdateControl(const pru_count_t* c, pru_mem_t* mem);
void Pru0Cleanup(void);
void Pru1Init(void);
void Pru1UpdateState(const pru_count_t* c, pru_mem_t* mem);
void Pru1UpdateControl(const pru_count_t* c, pru_mem_t* mem);
void Pru1Cleanup(void);

#endif
