#include "udf.h"
#include "emg.h"


void init_pru0(void)
{
//  emgInit();
}

void init_pru1(void)
{
}

void updateState_pru0(uint32_t cnt, uint32_t si)
{
  s->state[si].emg_raw = 1;
//  s->state[si].emg_raw = emgSampleBicep();
}

void updateState_pru1(uint32_t cnt, uint32_t si)
{
}

void updateControl_pru0(uint32_t cnt, uint32_t si)
{
}

void updateControl_pru1(uint32_t cnt, uint32_t si)
{
}

void cleanup_pru0(void)
{
//  emgCleanup();
}

void cleanup_pru1(void)
{
}


