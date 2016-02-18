#include <stdint.h>

#include "pwmdriver.h"
#include "maxonmotor.h"

void motorInit(void)
{
  motorDisable();
  pwmInit();
  motorSetDir(0);
  pwmSetCmpValue(duty2cmpval(MIN_DUTY));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  motorSetDir(0);
  pwmCleanUp();
}


void motorSetDuty(int16_t current_cmd, volatile int32_t *prev_cmd,
                  volatile int32_t *motorDuty)
{
  /* Convert current -> duty
   * 10% -- 0 A
   * 90% -- 15 A
   */
  float scaling = 5.33; // 80/15

  int16_t duty;
  uint16_t abs_duty;
  int16_t sign = 0;


  // Positive cmd and positive prev_cmd (no direction change)
  if ((current_cmd >= 0) && (*prev_cmd >= 0)){
    sign = 1;
  }
  // Negative cmd and negative prev_cmd (no direction change)
  else if ((current_cmd < 0) && (*prev_cmd < 0)){
    sign = -1;
  }
  // Positive cmd and negative prev_cmd (direction change)
  else if ((current_cmd >= 0) && (*prev_cmd < 0)){
    sign = 1;
    motorDisable();
    pwmSetCmpValue(duty2cmpval(MIN_DUTY));
    motorSetDir(0);
  }
  // Negative cmd and positive prev_cmd (direction change)
  else if ((current_cmd < 0) && (*prev_cmd >= 0)){
    sign = -1;
    motorDisable();
    pwmSetCmpValue(duty2cmpval(MIN_DUTY));
    motorSetDir(1);
  }


//  if (current_cmd >= 0){
////    motorSetDir(0);
//    sign = 1;
//  }
//  else{
////    motorSetDir(1);
//    sign = -1;
//  }

  duty = (int16_t) ((float)sign * scaling *(float)current_cmd) + 10;
  abs_duty = (uint16_t)abs(duty);

  if(abs_duty < MIN_DUTY)
    abs_duty = MIN_DUTY;
  else if(abs_duty > MAX_DUTY)
    abs_duty = MAX_DUTY;

//  if (sign == 1)
//    motorSetDir(0);
//  else
//    motorSetDir(1);

  pwmSetCmpValue(duty2cmpval(abs_duty));
  motorEnable();

  *motorDuty = sign * (int32_t)abs_duty;
  *prev_cmd = *motorDuty;
}

void motorEnable(void)
{
  /* Active Low */
  __R30 &= ~(1 << ENABLE_PIN);
}

void motorDisable(void)
{
  /* Active Low */
  __R30 |= (1 << ENABLE_PIN);
}

void motorSetDir(uint8_t dorsiflex)
{
  /* High -> CW motor rotation -> plantarflex */

  if(dorsiflex){
    __R30 &= ~(1 << DIR_PIN);
  }
  else{
     __R30 |= (1 << DIR_PIN);
  }
}

uint16_t duty2cmpval(uint16_t duty)
{
  return (uint16_t)(-100*((uint16_t)duty) + 10000);
}

