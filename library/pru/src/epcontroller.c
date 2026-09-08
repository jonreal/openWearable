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

#include "epcontroller.h"
#include <stdlib.h>

// Clamp to [0, hi].
static fix16_t clamp0(fix16_t x, fix16_t hi) {
  if (x < 0)  return 0;
  if (x > hi) return hi;
  return x;
}

epcontroller_t* EpControllerInit(fix16_t Pmax) {
  epcontroller_t* c = malloc(sizeof(epcontroller_t));
  c->Pmax    = Pmax;
  c->theta_d = 0;
  c->k       = 0;
  c->stiff   = 0;
  c->ep      = 0;
  c->Pd1     = 0;
  c->Pd2     = 0;
  return c;
}

void EpControllerSetEp(epcontroller_t* c, fix16_t theta_d) { c->theta_d = theta_d; }
void EpControllerSetK(epcontroller_t* c, fix16_t k)        { c->k = k; }
void EpControllerSetPmax(epcontroller_t* c, fix16_t Pmax)  { c->Pmax = Pmax; }

void EpControllerUpdate(epcontroller_t* c) {
  // Pure EP/impedance law over CLEAN inputs (the caller conditions its signal --
  // e.g. the pots filter their own noise): co-contraction +/- signed offset.
  c->stiff = fix16_smul(c->k, c->Pmax);             // 0 .. Pmax
  c->ep    = fix16_smul(c->theta_d, c->stiff);      // theta_d in [-1,1] -> +/- stiff
  c->Pd1   = clamp0(fix16_sadd(c->stiff, c->ep), c->Pmax);
  c->Pd2   = clamp0(fix16_ssub(c->stiff, c->ep), c->Pmax);
}

fix16_t EpControllerGetPd1(const epcontroller_t* c) { return c->Pd1; }
fix16_t EpControllerGetPd2(const epcontroller_t* c) { return c->Pd2; }

void EpControllerFree(epcontroller_t* c) { free(c); }
