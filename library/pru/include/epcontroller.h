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

#ifndef _EPCONTROLLER_H_
#define _EPCONTROLLER_H_

#include <stdint.h>
#include "fix16.h"
#include "filter.h"

// Equilibrium-point / impedance controller for an antagonist PAM pair.
//
// Pure block: setpoints in -- an equilibrium point theta_d (differential, the
// virtual spring's rest offset) and a stiffness k (co-contraction) -- and the two
// antagonist pressure setpoints Pd1/Pd2 out, scaled by a ceiling Pmax:
//
//     stiff = k * Pmax                 (co-contraction, 0..Pmax)
//     ep    = theta_d * stiff          (signed offset, +/- stiff)
//     Pd1   = clamp(stiff + ep, 0, Pmax)
//     Pd2   = clamp(stiff - ep, 0, Pmax)
//
// It owns NO actuator: userspace applies Pd1/Pd2 to the PAMs, and a reflex may
// edit pam->pd on top (they compose as sparse, HOLD-gated editors). The setpoints
// are input-agnostic -- drive theta_d/k from pots today, a web slider or ML
// output tomorrow -- and optional per-input filters ramp any command in smoothly.
// See notes/ep-controller-module.md.
typedef struct {
  fix16_t Pmax;          // pressure ceiling
  fix16_t theta_d;       // EP setpoint target,  [-1, 1]  (differential)
  fix16_t k;             // stiffness target,     [0, 1]  (co-contraction)
  iir_filt_t* filt_ep;   // optional smoothing on theta_d (NULL = passthrough)
  iir_filt_t* filt_k;    // optional smoothing on k       (NULL = passthrough)
  fix16_t theta_f;       // filtered theta_d
  fix16_t k_f;           // filtered k
  fix16_t stiff;         // k_f * Pmax   (kept for logging)
  fix16_t ep;            // theta_f*stiff (kept for logging)
  fix16_t Pd1, Pd2;      // outputs: antagonist pressure setpoints
} epcontroller_t;

epcontroller_t* EpControllerInit(fix16_t Pmax, iir_filt_t* filt_ep, iir_filt_t* filt_k);
void    EpControllerSetEp(epcontroller_t* c, fix16_t theta_d);  // EP target   [-1, 1]
void    EpControllerSetK(epcontroller_t* c, fix16_t k);         // stiffness   [ 0, 1]
void    EpControllerSetPmax(epcontroller_t* c, fix16_t Pmax);
void    EpControllerUpdate(epcontroller_t* c);                  // -> Pd1, Pd2 (filtered)
fix16_t EpControllerGetPd1(const epcontroller_t* c);
fix16_t EpControllerGetPd2(const epcontroller_t* c);
void    EpControllerFree(epcontroller_t* c);

#endif
