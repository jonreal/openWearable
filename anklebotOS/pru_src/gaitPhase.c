#include <stdint.h>

#include "mem_types.h"
#include "gaitPhase.h"


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
  gp.prevAvgPeriodCnts = 0;
  gp.periodCnts[0] = 0;
  gp.periodCnts[1] = 0;
  gp.periodCnts[2] = 0;
  gp.HS_cntStamp = 0;
  gp.prevHS_cntStamp = 0;
  gp.heelStrikeCnt = 0;
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

void resetGaitPhase(void)
{
  gp.prevGaitPhase = 0;
  gp.prevAvgPeriodCnts = 0;
  gp.periodCnts[0] = 0;
  gp.periodCnts[1] = 0;
  gp.periodCnts[2] = 0;
  gp.HS_cntStamp = 0;
  gp.prevHS_cntStamp = 0;
  gp.heelStrikeCnt = 0;
}

uint8_t isGaitPhaseReady(void)
{
  if(gp.heelStrikeCnt > 4)
    return 1;
  else
    return 0;
}

void gaitPhaseDetect(volatile uint32_t cnt,
                     volatile uint16_t *gaitPhase,
                     volatile uint16_t *avgPeriod,
                     volatile uint32_t *heelStrikeCnt,
                     volatile uint16_t *adc)
{
  uint32_t cntsSinceLastHS = cnt - gp.prevHS_cntStamp;
  uint32_t debounceCntsStance = 800;
  uint32_t debounceCntsSwing = 800;

  /* 0 - unknown
   * 1 - stance
   * 2 - preswing
   * 3 - swing */

  /* Unkown, wait for HS */
  if(gp.prevGaitPhase == 0){
    if(isHeelStrike(adc)){
      gp.HS_cntStamp = cnt;
      *gaitPhase = 1;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.HS_cntStamp;
    }
    else {
      *gaitPhase = 0;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
  }

  /* Stance, wait for TO */
  else if(gp.prevGaitPhase == 1){
    if(isToeOff(adc) & (cntsSinceLastHS > debounceCntsStance)){
      *gaitPhase = 2;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
  }

  /* Swing, wait for HS */
  else if(gp.prevGaitPhase == 2){
    if(isHeelStrike(adc) & (cntsSinceLastHS > debounceCntsSwing)){

      /* Cnt Stamp */
      gp.HS_cntStamp = cnt;

      /* Circular Buffer for average period */
      gp.periodCnts[2] = gp.periodCnts[1];
      gp.periodCnts[1] = gp.periodCnts[0];
      gp.periodCnts[0] = gp.HS_cntStamp - gp.prevHS_cntStamp;

      /* Average Period */
      *avgPeriod = (gp.periodCnts[0] + gp.periodCnts[1] + gp.periodCnts[2])/3;

      *gaitPhase = 1;

      if(gp.heelStrikeCnt < 5)
        gp.heelStrikeCnt++;

      *heelStrikeCnt = gp.HS_cntStamp;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
  }
  gp.prevGaitPhase = *gaitPhase;
  gp.prevAvgPeriodCnts = *avgPeriod;
  gp.prevHS_cntStamp = gp.HS_cntStamp;
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

