#include "udf.h"

// Include sensor/actuator/algorithmic drivers here
#include "emg.h"
#include "pressure.h"

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(void) {
//  emgInit();
//  pressureSensorInit();
}

void Pru0UpdateState(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem) {
//  pru_mem->s->state[si].emg_raw = emgSampleBicep();
}

void Pru0UpdateControl(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem) {
}

void Pru0Cleanup(void) {
  EmgCleanup();
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(void) {
}

void Pru1UpdateState(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem) {
}

void Pru1UpdateControl(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem) {
}

void Pru1Cleanup(void) {
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
