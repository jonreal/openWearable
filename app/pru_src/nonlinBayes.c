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

#include "nonlinBayes.h"


void nonlinBayesFiltInit(nonlinBayes_t* nlb, fix16_t alphaIn, fix16_t betaIn)
{
  nlb->alpha = alphaIn;
  nlb->beta = betaIn;

  for(int i=0; i<N_BAYESBINS; i++){
    nlb->prior[i] =
      fix16_sdiv(fix16_from_int(i+1),fix16_from_int(N_BAYESBINS));
  }
}

fix16_t nonlinBayesFilt(nonlinBayes_t* nlb, fix16_t in)
{


  return fix16_from_int(0xFF);
}
