#ifndef _MOTOR_H_
#define _MOTOR_H_

#define MIN_DUTY    10.0
#define MAX_DUTY    90.0

#define ADC_OFFSET  10.0

typedef struct
{
  unsigned int enb;
  unsigned int dir;
  unsigned int cur_ai;
  unsigned int vel_ai;
} motor_gpio_t;

int init_motor(void);
int set_motor_duty(float duty);
int stop_motor(void);
int motor_cleanup(void);
int read_motor_current(volatile float *value);
int read_motor_velocity(volatile float *value);


#endif
