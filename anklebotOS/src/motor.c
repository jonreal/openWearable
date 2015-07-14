#include <stdlib.h>
#include <stdio.h>

#include "pinconfig.h"
#include "common.h"
#include "gpio.h"
#include "pwm.h"
#include "motor.h"

/* Stuct for pin config ---------------------------------------------------- */
motor_gpio_t *mp_ptr;

/* ------------------------------------------------------------------------- */
int init_motor(void)
{
  unsigned int gpio;

  /* Allocate */
  mp_ptr = malloc(sizeof(motor_gpio_t));

  /* set enable pins */
  if(get_gpio_number(ENB_PIN, &gpio) != 0){
    printf("Error getting gpio number.");
    return -1;
  }
  mp_ptr->enb = gpio;
  gpio_export(mp_ptr->enb);
  gpio_set_direction(mp_ptr->enb, OUTPUT);

  /* set direction pin */
  if(get_gpio_number(DIR_PIN, &gpio) != 0){
    printf("Error getting gpio number.");
    return -1;
  }
  mp_ptr->dir = gpio;
  gpio_export(mp_ptr->dir);
  gpio_set_direction(mp_ptr->dir, OUTPUT);

  /* active LOW */
  gpio_set_value(mp_ptr->enb, HIGH);
  gpio_set_value(mp_ptr->dir, HIGH);

  if(pwm_start(PWM_PIN, MIN_DUTY, 5000.0, 0) != 1){
    printf("PWM unable to start.");
    return -1;
  }
  return 0;
}


/* ------------------------------------------------------------------------- */
int set_motor_duty(float duty)
{
  if (duty > 0){
    gpio_set_value(mp_ptr->enb, LOW);
    gpio_set_value(mp_ptr->dir, HIGH);
    duty = duty + 10;
  }
  else{
    gpio_set_value(mp_ptr->enb, LOW);
    gpio_set_value(mp_ptr->dir, LOW);
    duty = duty - 10;
  }

  if(abs(duty) <  MIN_DUTY){
    pwm_set_duty_cycle(PWM_PIN, MIN_DUTY);
  }
  else if(abs(duty) > MAX_DUTY){
    pwm_set_duty_cycle(PWM_PIN, MAX_DUTY);
  }
  else{
    pwm_set_duty_cycle(PWM_PIN, abs(duty));
  }
  return 0;
}


/* ------------------------------------------------------------------------- */
int stop_motor(void)
{
  gpio_set_value(mp_ptr->enb, HIGH);
  gpio_set_value(mp_ptr->dir, HIGH);
  pwm_disable(PWM_PIN);
  return 0;
}

/* ------------------------------------------------------------------------- */
int motor_cleanup(void)
{
  stop_motor();
  exports_cleanup();
  pwm_cleanup();

  printf("Motor cleaned up.\n");
  return 0;
}
