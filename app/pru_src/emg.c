#include "emg.h"
#include "soc_AM33XX.h"
#include "adcdriver.h"

void emgInit(void)
{
  int a = 1;
  a = a + a;
}

void emgCleanup(void)
{

}

uint32_t emgSampleBicep(void)
{
  return adcSample(1);
}
