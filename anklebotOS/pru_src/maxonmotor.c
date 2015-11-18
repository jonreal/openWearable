#include <stdint.h>

#include "pwmdriver.h"
#include "maxonmotor.h"

void motorInit(void)
{
  motorDisable();
  pwmInit();
  motorSetDir(1);
  pwmSetCmpValue(duty2cmpval(10));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  motorSetDir(0);
  pwmCleanUp();
}


void motorSetDuty(int16_t duty, volatile int16_t *motorDuty)
{
  uint16_t abs_duty = (uint16_t)abs(duty);
  int16_t sign = 0;

  if(abs_duty < MIN_DUTY)
    abs_duty = MIN_DUTY;
  else if(abs_duty > MAX_DUTY)
    abs_duty = MAX_DUTY;

  if(duty > 0){
    motorSetDir(0);
    sign = 1;
  }
  else{
    motorSetDir(1);
    sign = -1;
  }

  pwmSetCmpValue(duty2cmpval(abs_duty));
  *motorDuty = sign*((int16_t)abs_duty);
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

