#include <stdint.h>

#include "pwmdriver.h"
#include "fix16.h"
#include "maxonmotor.h"

void motorInit(void)
{
  motorDisable();
  pwmInit();
  motorSetDir(0);
  pwmSetCmpValue(duty2cmpval(fix16_from_int(MIN_DUTY)));
  motorEnable();
}

void motorCleanUp(void)
{
  motorDisable();
  motorSetDir(0);
  pwmCleanUp();
}


void motorSetDuty(fix16_t u, volatile uint32_t *pwmCycleCnt,
                  volatile int32_t *prev_cmd,
                  volatile int32_t *motorDuty)
{
  /* Convert current -> duty
   * 10% -- 0 A
   * 90% -- 15 A
   * duty = +-80/15 * i + 10
   */

  fix16_t k = fix16_from_float(5.33); // 80/15 (slope)
  fix16_t k_neg = fix16_from_float(-5.33);
  fix16_t duty, absDuty;
  int16_t sign = 0;


  // Positive cmd and positive prev_cmd (no direction change)
  if ((u >= 0) && (*prev_cmd >= 0)){
    sign = 1;
  }
  // Negative cmd and negative prev_cmd (no direction change)
  else if ((u < 0) && (*prev_cmd < 0)){
    sign = -1;
  }
  // Positive cmd and negative prev_cmd (direction change)
  else if ((u >= 0) && (*prev_cmd < 0)){
    sign = 1;
    pwmSetCmpValue(duty2cmpval(fix16_from_int(MIN_DUTY)));
    motorSetDir(0);
    // Reset Cycle count;
    *pwmCycleCnt = 0;
  }
  // Negative cmd and positive prev_cmd (direction change)
  else if ((u < 0) && (*prev_cmd >= 0)){
    sign = -1;
    pwmSetCmpValue(fix16_from_int(duty2cmpval(MIN_DUTY)));
    motorSetDir(1);
    *pwmCycleCnt = 0;
  }

  // Use fix point math
  if (sign == 1)
    duty = fix16_sadd(fix16_smul(k, u), fix16_from_int(10));
  else
    duty = fix16_sadd(fix16_smul(k_neg, u), fix16_from_int(10));

  absDuty = (duty < 0 ? -duty : duty);

  if(absDuty < fix16_from_int(MIN_DUTY))
    absDuty = fix16_from_int(MIN_DUTY);
  else if(absDuty > fix16_from_int(MAX_DUTY))
    absDuty = fix16_from_int(MAX_DUTY);

//  if(*pwmCycleCnt >= 4)
//    pwmSetCmpValue(duty2cmpval(absDuty));
//  else
//    pwmSetCmpValue(duty2cmpval(fix16_from_int(MIN_DUTY)));

    pwmSetCmpValue(duty2cmpval(absDuty));

  *motorDuty = sign * fix16_to_int(duty);
  *prev_cmd = *motorDuty;
  *pwmCycleCnt = ((*pwmCycleCnt+1) % 6);
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

uint16_t duty2cmpval(fix16_t duty)
{
  // 100% duty at 0 cmp
  // 0% duty at 10000 cmp
  // cmp = -100 * duty + 10000
  // duty =  k * i + c

  fix16_t k = fix16_from_int(-100);
  fix16_t c = fix16_from_int(10000);

  return (uint16_t) fix16_to_int(fix16_sadd(fix16_smul(k, duty), c));
}

