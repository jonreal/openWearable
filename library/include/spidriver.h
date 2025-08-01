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

#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debug_buff;

// Prototypes -----------------------------------------------------------------
void spiInit(void);
void spiCleanUp(void);
uint32_t spiXfer(uint8_t channel, uint32_t tx);
void spiForceBeginXfer(void);
void spiForceEndXfer(void);
void spiMultiWordXfer(uint32_t* tx, uint32_t* rx, uint32_t len);


#endif
