#ifndef _SOLENOID_H_
#define _SOLENOID_H_

#include "stdint.h"

typedef enum {
  CLOSE = 0,
  OPEN = 1,
} solenoid_state_t;

void SolenoidSetState(volatile register uint32_t pru_io,
                      const uint32_t valve_pin,
                      solenoid_state_t STATE);

#endif /* _SOLENOID_H_ */
