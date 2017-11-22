#include "udf.h"

// Include sensor/actuator drivers here
#include "emg.h"

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void pru0Init(void)
{
  emgInit();
}

void pru0UpdateState(uint32_t cnt, uint32_t si)
{
  s->state[si].emg_raw = emgSampleBicep();
}

void pru0UpdateControl(uint32_t cnt, uint32_t si)
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

void pru1UpdateState(uint32_t cnt, uint32_t si)
{
}

void pru1UpdateControl(uint32_t cnt, uint32_t si)
{
}

void pru1Cleanup(void)
{
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
