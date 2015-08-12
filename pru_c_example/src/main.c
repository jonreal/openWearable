#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define OFFSET_SHAREDRAM  2048
#define PRU_NUM     0
#define AM33XX

typedef struct{
  volatile uint32_t timeStamp;
  volatile uint32_t ch[7];
} adc_t;


int pru_run(const char *const path)
{
  int rtn = 0;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;

  /* Check Path */
  if(!path){
    printf("pru_setup(): path is NULL\n");
    return -1;
  }

  /* Initialize Interrupt */
  if( (rtn = prussdrv_pruintc_init(&intc)) != 0){
    printf("prussdrv_pruintc_init() failed with %i.\n", rtn);
    return rtn;
  }

  /* Load and run PRU program */
  if( (rtn = prussdrv_exec_program(PRU_NUM, path)) != 0){
    printf("prussdrv_exec_program() failed with %i.\n", rtn);
    return rtn;
  }

  return 0;
}

static int pru_setup(void)
{
  int rtn = 0;

  /* Initialize PRU */
  if( (rtn = prussdrv_init()) != 0){
    printf("prusdrv_init() failed with %i.\n", rtn);
    return rtn;
  }

  /* Open interrupt */
  if( (rtn = prussdrv_open(PRU_EVTOUT_0)) != 0){
    printf("prussdrv_open() failed with %i.\n", rtn);
    return rtn;
  }

   return 0;
}

static int pru_cleanup(void)
{
  int rtn = 0;

  /* clear the event (if asserted) */
  if(prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT)) {
    printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
    rtn = -1;
   }

  /* halt and disable the PRU (if running) */
  if( (rtn = prussdrv_pru_disable(PRU_NUM)) != 0) {
    printf("prussdrv_pru_disable() failed with %i\n", rtn);
    rtn = -1;
   }

  /* release the PRU clocks and disable prussdrv module */
  if( (rtn = prussdrv_exit()) != 0) {
    fprintf(stderr, "prussdrv_exit() failed\n");
    rtn = -1;
  }

  return rtn;
}

int main(int argc, char **argv)
{
  int rtn = 0;
  void *sharedMem;
  adc_t *adc;

  printf("-----------------\n");
  printf("- PRU c example -\n");
  printf("-----------------\n\n");

  struct timeval tv;
  struct tm* ptm;

  /* initialize the library, PRU and interrupt */
  if((rtn = pru_setup()) != 0){
    return -1;
  }

  /* Memory map for shared dataram */
  if( (rtn = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &sharedMem)) != 0){
    printf("prussdrv_map_prumem() failed with %i.\n", rtn);
    return -1;
  }
  adc = (adc_t*) sharedMem;

  /* Run PRU software */
  if( (rtn = pru_run("./bin/pru_main_text.bin")) != 0){
    printf("pru_run() failed with %i\n", rtn);
    return -1;
  }

  for(int i=0; i<5; i++){
    printf("waiting for interrupt from PRU0...\n");

    /* wait for PRU to assert the interrupt to indicate completion */
    int intNum = prussdrv_pru_wait_event(PRU_EVTOUT_0);

    /* clear the event (if asserted) */
    if( (rtn = prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT))) {
      printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
      rtn = -1;
    }
    printf("PRU0 interrupt, %i.\n",intNum);

    printf("TimeStamp = %d\n", adc->timeStamp);
    printf("ADC CH[0] = %d\n", adc->ch[0]);
    printf("ADC CH[1] = %d\n", adc->ch[1]);
    printf("ADC CH[2] = %d\n", adc->ch[2]);
    printf("ADC CH[3] = %d\n", adc->ch[3]);
    printf("ADC CH[4] = %d\n", adc->ch[4]);
    printf("ADC CH[5] = %d\n", adc->ch[5]);
    printf("ADC CH[6] = %d\n", adc->ch[6]);
  }

  /* Cleanup */
  printf("Pru cleanup!\n");
  pru_cleanup();

  return 0;
}


