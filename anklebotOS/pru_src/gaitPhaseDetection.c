#include <stdint.h>

#include "mem_types.h"
#include "gaitPhaseDetection.h"


/* Stuct */
gp_t gp;


void gaitPhaseInit(param_mem_t* params)
{
  gp.toe_hs = params->gp_toe_hs;
  gp.mid_hs = params->gp_mid_hs;
  gp.heel_hs = params->gp_heel_hs;
  gp.toe_to = params->gp_toe_to;
  gp.mid_to = params->gp_mid_to;
  gp.heel_to = params->gp_heel_to;

  if(params->gpOnLeftFoot){
    gp.toe_indx = 2;
    gp.mid_indx = 3;
    gp.heel_indx = 4;
  }
  else{
    gp.toe_indx = 5;
    gp.mid_indx = 6;
    gp.heel_indx = 7;
  }

  gp.prevGaitPhase = 0;
  gp.prevAvgPeriod = 0;
  gp.period[0] = 0;
  gp.period[1] = 0;
  gp.period[2] = 0;
}

void gaitPhaseUpdateParams(param_mem_t *params)
{
  gp.toe_hs = params->gp_toe_hs;
  gp.mid_hs = params->gp_mid_hs;
  gp.heel_hs = params->gp_heel_hs;
  gp.toe_to = params->gp_toe_to;
  gp.mid_to = params->gp_mid_to;
  gp.heel_to = params->gp_heel_to;
}

void gaitPhaseDetect(volatile uint32_t cnt,
                     volatile uint16_t *gaitPhase,
                     volatile uint16_t *avgPeriod,
                     volatile uint16_t *adc)
{
  /* 0 - unknown
   * 1 - stance
   * 2 - swing */

  /* Unkown, wait for HS */
  if(gp.prevGaitPhase == 0){
    if(isHeelStrike(adc)){
      *gaitPhase = 1;
      *avgPeriod = gp.prevAvgPeriod;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriod;
    }
  }

  /* Stance, wait for TO */
  else if(gp.prevGaitPhase == 1){
    if(isToeOff(adc)){
      *gaitPhase = 2;
      *avgPeriod = gp.prevAvgPeriod;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriod;
    }
  }

  /* Swing, wait for HS */
  else if(gp.prevGaitPhase == 2){
    if(isHeelStrike(adc)){
      *gaitPhase = 1;

      /* Circular Buffer for average period */
      gp.period[2] = gp.period[1];
      gp.period[1] = gp.period[0];
      gp.period[0] = cnt - gp.period[0];

      /* Average Period */
      *avgPeriod = (gp.period[0] + gp.period[1] + gp.period[2])/3;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriod;
    }
  }
  gp.prevGaitPhase = *gaitPhase;
  gp.prevAvgPeriod = *avgPeriod;
}


uint16_t isHeelStrike(volatile uint16_t* adc)
{
  if( (adc[gp.heel_indx] > gp.heel_hs)
        & (adc[gp.mid_indx] < gp.mid_hs)
        & (adc[gp.toe_indx] < gp.toe_hs) ){
    return 1;
  }
  else{
    return 0;
  }
}

uint16_t isToeOff(volatile uint16_t* adc)
{
  if( (adc[gp.toe_indx] < gp.toe_to)
        & (adc[gp.heel_indx] < gp.heel_to)
        & (adc[gp.mid_indx] < gp.mid_to) ){
    return 1;
  }
  else{
    return 0;
  }
}
