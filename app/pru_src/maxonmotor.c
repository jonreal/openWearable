#include <stdint.h>

#include "pwmdriver.h"
#include "fix16.h"
#include "maxonmotor.h"

// Motor used analog set point (via filtered PWM)
//
// Motor Setting on ESCON Studio
// 14 amps -> 3.5 V
// -14 amps -> 0 V
//
// Experimental fit:
// cmpValue = K * current_d + B
//
// K = 384.9
// B = 4876

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

  if (u > FIX16_MAX_CURRENT){
    u = FIX16_MAX_CURRENT;
  }
  else if (u < -FIX16_MIN_CURRENT) {
    u = -FIX16_MIN_CURRENT;
  }

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
