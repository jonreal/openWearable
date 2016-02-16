#include <stdint.h>

#include "mem_types.h"
#include "gaitPhase.h"


/* Stuct */
gp_t gp;


void gaitPhaseInit(param_mem_t* params)
{
 // gp.toe_hs = params->gp_toe_hs;
 // gp.mid_hs = params->gp_mid_hs;
 // gp.heel_hs = params->gp_heel_hs;
 // gp.toe_to = params->gp_toe_to;
 // gp.mid_to = params->gp_mid_to;
 // gp.heel_to = params->gp_heel_to;

 // if(params->gpOnLeftFoot == 1){
 //   gp.toe_indx = 2;
 //   gp.mid_indx = 3;
 //   gp.heel_indx = 4;
 // }
 // else{
 //   gp.toe_indx = 5;
 //   gp.mid_indx = 6;
 //   gp.heel_indx = 7;
 // }

 // gp.prevGaitPhase = 0;
 // gp.prevAvgPeriodCnts = 0;
 // gp.periodCnts[0] = 0;
 // gp.periodCnts[1] = 0;
 // gp.periodCnts[2] = 0;
 // gp.HS_cntStamp = 0;
 // gp.prevHS_cntStamp = 0;
 // gp.heelStrikeCnt = 0;
}

void gaitPhaseUpdateParams(param_mem_t *params)
{
 // gp.toe_hs = params->gp_toe_hs;
 // gp.mid_hs = params->gp_mid_hs;
 // gp.heel_hs = params->gp_heel_hs;
 // gp.toe_to = params->gp_toe_to;
 // gp.mid_to = params->gp_mid_to;
 // gp.heel_to = params->gp_heel_to;
}

void resetGaitPhase(void)
{
 // gp.prevGaitPhase = 0;
 // gp.prevAvgPeriodCnts = 0;
 // gp.periodCnts[0] = 0;
 // gp.periodCnts[1] = 0;
 // gp.periodCnts[2] = 0;
 // gp.HS_cntStamp = 0;
 // gp.prevHS_cntStamp = 0;
 // gp.heelStrikeCnt = 0;
}

uint8_t isGaitPhaseReady(void)
{
 // if(gp.heelStrikeCnt > 4)
 //   return 1;
 // else
 //   return 0;
}

void heelStrikeDetect(volatile uint32_t cnt,
                      volatile uint16_t *avgPeriod,
                      volatile uint32_t *heelStrikeCnt,
                      volatile uint16_t *adc)
{
  /* 0 - heel sensor not pushed
   * 1 - heel sensor pushed
   */

  /* Unkown, wait for heel contact */
  if(gp.prevGaitPhase == 0){
    if( (isHeelContact(adc)) && !(isMidContact(adc)) && !(isToeContact(adc)) ){
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

  /* GP=1 (Stance), wait for toe contact */
  else if(gp.prevGaitPhase == 1){
    if(isToeContact(adc)){
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

  /* GP=2 (pre-swing), wait for no contact */
  else if(gp.prevGaitPhase == 2){
    if( !(isHeelContact(adc)) && !(isToeContact(adc)) ){
      *gaitPhase = 3;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
    else {
      *gaitPhase = gp.prevGaitPhase;
      *avgPeriod = gp.prevAvgPeriodCnts;
      *heelStrikeCnt = gp.prevHS_cntStamp;
    }
  }

  /* GP=3 (swing), wait for heel contact */
  else if(gp.prevGaitPhase == 3){
    if( (isHeelContact(adc)) && !(isMidContact(adc)) && !(isToeContact(adc)) ){

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

uint8_t isHeelContact(volatile int16_t heel, volatile int16_t d_heel)
{
  if()
    return 1;
  else
    return 0;
}

uint8_t isMidContact(volatile uint16_t* adc)
{
  if( adc[gp.mid_indx] > gp.mid_hs )
    return 1;
  else
    return 0;
}

uint8_t isToeContact(volatile uint16_t* adc)
{
  if( adc[gp.toe_indx] > gp.toe_hs )
    return 1;
  else
    return 0;
}

//uint16_t isHeelStrike(volatile uint16_t* adc)
//{
//  if( (adc[gp.heel_indx] > gp.heel_hs)
//        & (adc[gp.mid_indx] < gp.mid_hs)
//        & (adc[gp.toe_indx] < gp.toe_hs) ){
//    return 1;
//  }
//  else{
//    return 0;
//  }
//}
//
//uint16_t isToeOff(volatile uint16_t* adc)
//{
//  if( (adc[gp.toe_indx] < gp.toe_to)
//        & (adc[gp.heel_indx] < gp.heel_to)
//        & (adc[gp.mid_indx] < gp.mid_to) ){
//    return 1;
//  }
//  else{
//    return 0;
//  }
//}
//
