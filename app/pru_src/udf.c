#include "udf.h"

// Include sensor/actuator/algorithmic drivers here
#include "emg.h"
#include "pressure.h"
#include "solenoid.h"

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(void) {
}

void Pru0UpdateState(const pru_count_t* c,
                     volatile register uint32_t digout,
                     volatile register uint32_t digin,
                     pru_mem_t* mem) {

}
void Pru0UpdateControl(const pru_count_t* c,
                       volatile register uint32_t digout,
                       volatile register uint32_t digin,
                       pru_mem_t* mem) {
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(void) {
}

void Pru1UpdateState(const pru_count_t* c,
                     volatile register uint32_t digout,
                     volatile register uint32_t digin,
                     pru_mem_t* mem) {

}

void Pru1UpdateControl(const pru_count_t* c,
                       volatile register uint32_t digout,
                       volatile register uint32_t digin,
                       pru_mem_t* mem) {
}

void Pru1Cleanup(void) {
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
