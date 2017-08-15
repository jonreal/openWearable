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

#include <stdint.h>

#include "gaitPhase.h"



void gaitPhaseInit(void)
{
  for (int i=1; i>3; i++){
    p->l_period[i] = 0;
    p->r_period[i] = 0;
  }

  p->numOfSteps = 0;
  p->gaitDetectReady = 0;

  p->l_prevGaitPhase= 0;
  p->r_prevGaitPhase = 0;
  p->l_prevHsStamp = 0;
  p->r_prevHsStamp = 0;
  p->l_prevPeriod = 0;
  p->r_prevPeriod = 0;
}

void leftGaitPhaseDetect(uint32_t cnt,
                         volatile int16_t heelForce,
                         volatile int16_t d_heelForce)
{
  uint32_t gp = 0;
  uint32_t hsStamp = 0;
  uint32_t meanPeriod = 0;

  if (p->l_prevGaitPhase == 0){
    if ((d_heelForce > p->l_d_forceThrs) && (heelForce < p->l_forceThrs)){
      gp = 1;
      hsStamp = cnt;

      // Find mean period
      p->l_period[2] = p->l_period[1];
      p->l_period[1] = p->l_period[0];
      p->l_period[0] = fix16_from_int(hsStamp - p->l_prevHsStamp);

      meanPeriod = (uint32_t) fix16_to_int(
        fix16_sdiv(fix16_sadd(fix16_sadd(p->l_period[0],
                      p->l_period[1]), p->l_period[2]), fix16_from_int(3)));

      if (p->isProsLeft) {
      // Increment step
        p->numOfSteps++;
        if (p->numOfSteps > 5)
          p->numOfSteps = 5;

        if (p->numOfSteps >= 5)
          p->gaitDetectReady = 1;
      }

    }
    else{
      gp = 0;
      hsStamp = p->l_prevHsStamp;
      meanPeriod = p->l_prevPeriod;
    }
  }
  else if (p->l_prevGaitPhase == 1){
    if ((d_heelForce < (-p->l_d_forceThrs)) && (heelForce > p->l_forceThrs)){
      gp = 0;
      hsStamp = p->l_prevHsStamp;
      meanPeriod = p->l_prevPeriod;

    }
    else{
      gp = 1;
      hsStamp = p->l_prevHsStamp;
      meanPeriod = p->l_prevPeriod;
    }
  }

  // Store Results
  p->l_prevPeriod = meanPeriod;
  p->l_prevGaitPhase = gp;
  p->l_prevHsStamp = hsStamp;
}

void rightGaitPhaseDetect(uint32_t cnt,
                         volatile int16_t heelForce,
                         volatile int16_t d_heelForce)
{
  uint32_t gp = 0;
  uint32_t hsStamp = 0;
  uint32_t meanPeriod = 0;

  if (p->r_prevGaitPhase == 0){
    if ((d_heelForce > p->r_d_forceThrs) && (heelForce < p->r_forceThrs)){
      gp = 1;
      hsStamp = cnt;

      // Find mean period
      p->r_period[2] = p->r_period[1];
      p->r_period[1] = p->r_period[0];
      p->r_period[0] = fix16_from_int(hsStamp - p->r_prevHsStamp);

      meanPeriod = (uint32_t) fix16_to_int(
        fix16_sdiv(fix16_sadd(fix16_sadd(p->r_period[0],
                      p->r_period[1]), p->r_period[2]), fix16_from_int(3)));


      if (!p->isProsLeft){
        // Increment step
        p->numOfSteps++;
        if (p->numOfSteps > 5)
          p->numOfSteps = 5;

        if (p->numOfSteps >= 5)
          p->gaitDetectReady = 1;
      }

    }
    else{
      gp = 0;
      hsStamp = p->r_prevHsStamp;
      meanPeriod = p->r_prevPeriod;
    }
  }
  else if (p->r_prevGaitPhase == 1){
    if ((d_heelForce < (-p->r_d_forceThrs)) && (heelForce > p->r_forceThrs)){
      gp = 0;
      hsStamp = p->r_prevHsStamp;
      meanPeriod = p->r_prevPeriod;

    }
    else{
      gp = 1;
      hsStamp = p->r_prevHsStamp;
      meanPeriod = p->r_prevPeriod;
    }
  }

  // Store Results
  p->r_prevPeriod = meanPeriod;
  p->r_prevGaitPhase = gp;
  p->r_prevHsStamp = hsStamp;
}
