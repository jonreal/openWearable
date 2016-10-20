#include <stdint.h>

#include "pwmdriver.h"
#include "fix16.h"
#include "maxonmotor.h"

// Motor used analog set point (via filtered PWM)
//
// 20 amps -> 0 PWM cmpValue
// -20 amps -> 10000 PWM cmpValue
//
// cmpValue = K * current_d + B
// K & B are were found experiemtnally.

void motorInit(void)
{
  motorDisable();
  pwmInit();
  pwmSetCmpValue(motorCurrent2CmpValue(0));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  pwmSetCmpValue(motorCurrent2CmpValue(0));
  pwmCleanUp();
}

uint16_t motorCurrent2CmpValue(fix16_t u)
{
  int32_t temp = fix16_to_int(fix16_sadd(fix16_smul(FIX16_K,u),FIX16_B));
  return (uint16_t)temp;
}

void motorSetCurrent(fix16_t u, volatile uint32_t *motorPwmCmpValue)
{
  if (u > fix16_from_int(20))
    u = fix16_from_int(20);
  else if (u < fix16_from_int(-20))
    u = fix16_from_int(-20);

  uint16_t cmpValue = motorCurrent2CmpValue(u);
  pwmSetCmpValue(cmpValue);
  (*motorPwmCmpValue) = cmpValue;
}

void motorEnable(void)
{
  // Active Low
  __R30 &= ~(1 << ENABLE_PIN);
}

void motorDisable(void)
{
  // Active Low
  __R30 |= (1 << ENABLE_PIN);
}
