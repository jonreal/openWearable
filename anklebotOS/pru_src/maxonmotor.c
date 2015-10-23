#include <stdint.h>

#include "pwmdriver.h"
#include "maxonmotor.h"

void motorInit(void)
{
  motorDisable();
  pwmInit();
  pwmSetCmpValue(duty2cmpval(MIN_DUTY));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  motorSetDir(1);
  pwmCleanUp();
}


void motorSetDuty(float duty, volatile uint16_t *motorDuty)
{
  if(duty > 0)
    motorSetDir(0);
  else
    motorSetDir(1);

  __R30 ^= (1 << ENABLE_PIN);

  /* TODO: is this needed? */
  duty = abs(duty) + 10;

  if(duty < MIN_DUTY)
    duty = MIN_DUTY;
  else if(duty > MAX_DUTY)
    duty = MAX_DUTY;

  pwmSetCmpValue(duty2cmpval(duty));
  *motorDuty = (uint16_t)(duty*10.0);

  __R30 ^= (1 << ENABLE_PIN);
}

void motorEnable(void)
{
  /* Active Low */
//  __R30 &= ~(1 << ENABLE_PIN);
}

void motorDisable(void)
{
  /* Active Low */
//  __R30 |= (1 << ENABLE_PIN);
}

void motorSetDir(uint8_t dorsiflex)
{
  if(dorsiflex){
  //  __R30 |= (1 << DIR_PIN);
  }
  else{
  // __R30 &= ~(1 << DIR_PIN);
  }
}

uint16_t duty2cmpval(float duty)
{
  return (uint16_t) (-100.0*abs(duty) + 10000.0);
}

