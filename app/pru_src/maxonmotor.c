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



//void motorSetDuty(fix16_t u, volatile int32_t *motorDuty)
//{
//  /* Convert current -> duty
//   * 10% -- -20 A
//   * 90% -- 20 A
//   * duty = +-80/15 * i + 10
//   */
//  fix16_t duty, absDuty;
//  int16_t sign = 0;
//
//  if (u >= 0){
//    sign = 1;
//    motorSetDir(0);
//  }
//  else{
//    sign = -1;
//    motorSetDir(1);
//  }
//
//  // Use fix point math to convert current to duty
//  if (sign == 1)
//    duty = fix16_sadd(fix16_smul(FIX16_K, u), FIX16_10);
//  else
//    duty = fix16_sadd(fix16_smul(-FIX16_K, u), FIX16_10);
//
//  absDuty = (duty < 0 ? -duty : duty);
//
//  if (absDuty <= FIX16_MIN_DUTY)
//    absDuty = FIX16_MIN_DUTY;
//  else if (absDuty >= FIX16_MAX_DUTY)
//    absDuty = FIX16_MAX_DUTY;
//
//  pwmSetCmpValue(duty2cmpval(absDuty));
//  *motorDuty = sign * fix16_to_int(duty);
//}

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

//void motorSetDir(uint8_t dorsiflex)
//{
//  /* High -> CW motor rotation -> plantarflex */
//
//  if(dorsiflex){
//    __R30 &= ~(1 << DIR_PIN);
//  }
//  else{
//     __R30 |= (1 << DIR_PIN);
//  }
//}

uint16_t duty2cmpval(fix16_t duty)
{
  // 100% duty at 0 cmp
  // 0% duty at 10000 cmp
  // cmp = -100 * duty + 10000
  fix16_t cmpval = fix16_sadd(fix16_smul(-FIX16_100, duty), FIX16_10000);
  return (uint16_t) fix16_to_int(cmpval);
}

