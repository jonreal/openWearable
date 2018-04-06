#include "udf.h"

// Include sensor/actuator/algorithmic drivers here
#include "adcdriver.h"
#include "emg.h"
#include "pressure.h"
#include "solenoid.h"

#define MUX_SEL 6

#define FLEX_V_HP 8
#define FLEX_V_LP 10
#define EXT_V_HP 9
#define EXT_V_LP 11

#define BITS2FORCE 0x10000000
#define FORCEBIAS 0x8000000

volatile register uint32_t __R30;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(void) {
}

void Pru0UpdateState(const pru_count_t* c, pru_mem_t* mem) {

  uint32_t bits = adcSampleCh(0);
  mem->s->state[c->index].interaction_force =
    fix16_from_int((int16_t)bits - 2048);
}
void Pru0UpdateControl(const pru_count_t* c, pru_mem_t* mem) {
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(void) {
 // PressureSensorInit();
}

void Pru1UpdateState(const pru_count_t* c, pru_mem_t* mem) {

  __R30 |= (1 << MUX_SEL);
  mem->s->state[c->index].flexion_pressure = PressureSensorSample(SENSOR_1);
  __delay_cycles(500);

  __R30 &= ~(1 << MUX_SEL);
  __delay_cycles(500);
  mem->s->state[c->index].extension_pressure = PressureSensorSample(SENSOR_1);

}

void Pru1UpdateControl(const pru_count_t* c, pru_mem_t* mem) {
}

void Pru1Cleanup(void) {
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
