/* Copyright 2026 Jonathan Realmuto

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

#ifndef _CMD_H_
#define _CMD_H_

#include "mem_types.h"

// Host->board command channel (the "remote TUI"). A background thread receives
// ASCII lines "<name> <value>\n" on UDP :1501 (host->board, the second channel;
// telemetry goes out on :1500). It is off the real-time path.
//
// Safety: a SINGLE arm gate. "arm 1"/"arm 0" toggles ARM_ARMED in the shared arm
// word; every OTHER command is applied ONLY while armed. Disarmed at startup and
// whenever the host requests it (e.g. the web console auto-disarms on disconnect).
//
// The per-name mapping is app-specific, so CmdApply() is a per-app hook (like
// PruLoadParams): the library ships a weak no-op; an app overrides it to map
// names -> param_mem fields / owned-words command bits.
void CmdInit(pru_mem_t* pm);
void CmdCleanup(void);
void CmdApply(pru_mem_t* pm, const char* name, float value);

#endif /* _CMD_H_ */
