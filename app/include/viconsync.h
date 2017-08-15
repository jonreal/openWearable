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

#ifndef _VICONSYNC_H_
#define _VICONSYNC_H_

#define SYNC_PIN  (16)

/* Digital input reg. ------------------------------------------------------ */
volatile register uint32_t __R31;

/* Global ------------------------------------------------------------------ */ 
extern volatile uint32_t *debugBuffer;

/* Prototypes -------------------------------------------------------------- */
uint16_t viconSync(void);

#endif


