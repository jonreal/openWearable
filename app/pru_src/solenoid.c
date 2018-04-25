#include "solenoid.h"

volatile register uint32_t __R30;

void SolenoidSetState(const uint32_t valve_pin, solenoid_state_t STATE) {
  switch (STATE) {
    case CLOSE :
      __R30 &= ~(1 << valve_pin);
      break;
    case OPEN :
      __R30 |= (1 << valve_pin);
      break;
  }
}
