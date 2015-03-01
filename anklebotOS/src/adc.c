#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "adc.h"
#include "common.h"
#include "pruio.h"
#include "control.h"

pruIo *io;

int init_adc(void)
{
  io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);

  if(io->Errr){
    printf("ADC-pru initialization failed: %s",io->Errr);
    return -1;
  }
  if(pruio_config(io, 1, 0x1FE, 0, 4)){
    printf("ADC-pru configuration failed: %s",io->Errr);
    return -1;
  }
  add_func_to_cleanup(adc_cleanup);
  printf("ADC-pru initialized.\n");
  return 0;
}

int read_adc(int value[7])
{
  value[0] = io->Adc->Value[1];
  value[1] = io->Adc->Value[2];
  value[2] = io->Adc->Value[3];
  value[3] = io->Adc->Value[4];
  value[4] = io->Adc->Value[5];
  value[5] = io->Adc->Value[6];
  value[6] = io->Adc->Value[7];

  return 0;
}

int adc_cleanup(void)
{
  pruio_destroy(io);
  io = NULL;
  printf("ADC-pru cleanned up.\n");
  return 0;
}
