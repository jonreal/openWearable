#include "solenoid.h"

void SolenoidSetState(volatile register uint32_t pru_io,
                      const uint32_t valve_pin,
                      solenoid_state_t STATE) {
  switch (STATE) {
    case CLOSE :
      pru_io &= ~(1 << valve_pin);
      break;

    case OPEN :
      pru_io |= (1 << valve_pin);
      break;
  }
}
