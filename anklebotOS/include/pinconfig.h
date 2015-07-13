#ifndef _PINCONFIG_H_
#define _PINCONFIG_H_

#define PWM_PIN     "P8_13"
#define ENB_PIN     "P8_15"
#define DIR_PIN     "P8_17"

#define CUR_AIN     "P9_33"
#define VEL_AIN     "P9_35"

#define RAD2DEG(X)    ((180.0/M_PI)*X)
#define DEG2RAD(X)    ((M_PI/180.0)*X)

#define ANGLE_OFFSET DEG2RAD(-10)


#endif
