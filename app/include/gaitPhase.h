#ifndef _GAITPHASE_H_
#define _GAITPHASE_H_

#include "mem_types.h"

#define PROS_ON_LEFT 1

/* Global ------------------------------------------------------------------ */
extern volatile uint32_t *debugBuffer;
extern param_mem_t *p;

/* Prototypes ---------------------------------------------------------------*/
void gaitPhaseInit(void);
void leftGaitPhaseDetect(uint32_t cnt,
                         volatile int16_t heelForce,
                         volatile int16_t d_heelForce);
void rightGaitPhaseDetect(uint32_t cnt,
                         volatile int16_t heelForce,
                         volatile int16_t d_heelForce);
#endif
