#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "mem_types.h"
#include "pru_wrappers.h"

int doneFlag = 0;

void sigintHandler(int sig)
{
  signal(sig, SIG_IGN);
  doneFlag = 1;
}

int main(int argc, char **argv)
{
  int rtn = 0;
  uint8_t buffIndx = 0;
  state_t buff_cpy[SIZE_OF_BUFFS];

  const float freq_hz = (100.0);

  printf("Senosor loop = %f, (0x%x)\n", freq_hz,  hzToPruTicks(freq_hz));

  signal(SIGINT, sigintHandler);

  printf("-----------------\n");
  printf("- PRU c example -\n");
  printf("-----------------\n\n");

  /* initialize the library, PRU and interrupt */
  if((rtn = pru_init()) != 0){
    return -1;
  }
  printf("pru initialized.\n");

  /* Memory map for data/shared RAM */
  if( (rtn = pru_mem_init()) != 0){
    return -1;
  }
  printf("memory initialized.\n");

  /* Init sensor params */
  writePruSensorParams(freq_hz, 0xDEADBEAF, 0xBEAFDEAD, 0xFFFFFFFF);

  /* Init control params */
  uint32_t zeros[100] = {0,};
  writePruConrtolParams(10, 10, 10, zeros);
  printf("Params initialized.\n");

 /* Run PRU0 software */
  if( (rtn = pru_run(PRU_SENSOR, "./bin/pru0_sensor_text.bin")) != 0){
    printf("pru_run() failed (PRU_SENSOR)");
    return -1;
  }


  /* Run PRU1 software */
  if( (rtn = pru_run(PRU_CONTROL, "./bin/pru1_control_text.bin")) != 0){
    printf("pru_run() failed (PRU_CONTROL)");
    return -1;
  }



  printf("\nPress enter to begin data collection:\n ");
  getchar();

  /* Clear flow bit feild, and enable pru sensor/control */
  clearFlowBitFeild();
  enable();

  /* Start control loop */
  armToPru1Interrupt();
  armToPru0Interrupt();

  while(1){

    if(doneFlag)
      break;


    printf("waiting for full buffer...\n");

    while(1){
      if(isBufferFull()){
        resetBufferFullFlag();
        buffIndx = whichBuffer();
        break;
      }
      if(doneFlag)
        break;
    }

    readState(buff_cpy, buffIndx);

    printf("%i - %i\n",
           buff_cpy[0].timeStamp, buff_cpy[SIZE_OF_BUFFS-1].timeStamp);
    printf("%i - %i\n\n",
           buff_cpy[0].gaitPhase, buff_cpy[SIZE_OF_BUFFS-1].gaitPhase);
  }

  disable();

  sleep(1);

  /* Cleanup */
  pru_cleanup();
  printf("pru0 and pru1 cleaned up.\n");

  return 0;
}


