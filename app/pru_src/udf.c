#include "udf.h"

// Include sensor/actuator drivers here
#include "emg.h"
#include "pressureSensor.h"

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void pru0Init(void)
{
//  emgInit();
//  pressureSensorInit();
}

void pru0UpdateState(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem)
{
//  pru_mem->s->state[si].emg_raw = emgSampleBicep();
}

void pru0UpdateControl(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem)
{
}

void pru0Cleanup(void)
{
  emgCleanup();
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void pru1Init(void)
{
}

void pru1UpdateState(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem)
{
}

void pru1UpdateControl(uint32_t cnt, uint32_t si, pru_mem_t* pru_mem)
{
}

void pru1Cleanup(void)
{
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
