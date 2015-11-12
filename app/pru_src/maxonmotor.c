#include <stdint.h>

#include "pwmdriver.h"
#include "fix16.h"
#include "maxonmotor.h"

// Motor using analog set point (via filtered PWM)
//
// 20 amps -> 100% duty
// -20 amps -> 0% duty

void motorInit(void)
{
  motorDisable();
  pwmInit();
  pwmSetCmpValue(duty2cmpval(FIX16_50));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  pwmSetCmpValue(duty2cmpval(FIX16_50));
  pwmCleanUp();
}


void motorSetDuty(fix16_t u, volatile fix16_t *motorDuty)
{
  // Convert current to duty
  // duty = 5/2 * current + 50
  fix16_t duty = fix16_sadd(fix16_smul(-FIX16_K, u), FIX16_50);
  pwmSetCmpValue(duty2cmpval(duty));
  *motorDuty = duty;
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

uint16_t duty2cmpval(fix16_t duty)
{
  // 100% duty at 0 cmp
  // 0% duty at 10000 cmp
  // cmp = -100 * duty + 10000
  fix16_t cmpval = fix16_sadd(fix16_smul(-FIX16_100, duty), FIX16_10000);
  return (uint16_t) fix16_to_int(cmpval);
}

