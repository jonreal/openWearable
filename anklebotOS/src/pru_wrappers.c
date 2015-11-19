#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "mem_types.h"
#include "pru_wrappers.h"


#define M_PI (3.14159265359)

#define AM33XX

/* Pointer to state data */
shared_mem_t *p;

/* Pointer to sensor pru params */
param_mem_t *param;

/* Pointer to controller pru params */
lookUp_mem_t *lookUp;


void initDebugBuffer(void)
{
  for(int i=0; i<10; i++){
    param->debugBuffer[i] = 0;
  }
}

void printDebugBuffer(void)
{
  printf("\n\n---- Debug Buffer ----\n");
  for(int i=0; i<10; i++)
  {
    printf("0x%X\t%i\n", param->debugBuffer[i], param->debugBuffer[i]);
  }
}


/* ----------------------------------------------------------------------------
 * Function: int pru_run(const int pruNum, const char *const path)
 *
 * This function loads binary file into pru IRAM, and starts the SW.
 *
 * Inputs:    pruNum  -   which pru (0|1)
 *            path    -   path to binary file
 *
 * Outputs:   1 for success
 * ------------------------------------------------------------------------- */
int pru_run(const int pruNum, const char *const path)
{
  int rtn = 0;

  /* Load and run PRU program */
  if( (rtn = prussdrv_exec_program(pruNum, path)) != 0){
    printf("prussdrv_exec_program() failed with %i.\n", rtn);
    return rtn;
  }

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function: int pru_init(void)
 *
 * This function initializes pru library in linux space and opens 1 interrupt
 * (PRU_EVTOUT_0).
 *
 * Outputs:   1 for success
 * ------------------------------------------------------------------------- */
int pru_init(void)
{
  int rtn = 0;
  tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;

  /* Initialize PRU */
  if( (rtn = prussdrv_init()) != 0){
    printf("prusdrv_init() failed with %i.\n", rtn);
    return rtn;
  }

  /* Open interrupt PRU0 */
  if( (rtn = prussdrv_open(PRU_EVTOUT_0)) != 0){
    printf("prussdrv_open() failed with %i.\n", rtn);
    return rtn;
  }

  /* Open interrupt PRU1 */
  if( (rtn = prussdrv_open(PRU_EVTOUT_1)) != 0){
    printf("prussdrv_open() failed with %i.\n", rtn);
    return rtn;
  }

  /* Initialize Interrupt */
  if( (rtn = prussdrv_pruintc_init(&intc)) != 0){
    printf("prussdrv_pruintc_init() failed with %i.\n", rtn);
    return rtn;
  }

  /* Initialize memory */
  if( (rtn = pru_mem_init()) != 0){
    printf("pru_mem_init() failedi!\n");
    return -1;
  }

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function: int pru_cleanup(const int pruNum)
 *
 * This function clears event interrupts, diables pru and realeases pru clocks.
 *
 * Inputs:    pruNum    -   which pru (0|1)
 *
 * Outputs:   1 for success
 * ------------------------------------------------------------------------- */
int pru_cleanup(void)
{
  int rtn = 0;

  /* Send Interrupt to pru0 */
//  armToPru0Interrupt();
//  printf("Sent interrupt to pru0...\n");

  /* Wait for pru0 acknowledgement */
//  prussdrv_pru_wait_event(PRU_EVTOUT_0);
//  printf("Acknowledgement recieved, pru0 halted.\n");

  /* Send Interrupt to pru1 */
//  armToPru1Interrupt();
//  printf("Sent interrupt to pru1...\n");

  /* Wait for pru1 acknowledgement */
//  prussdrv_pru_wait_event(PRU_EVTOUT_1);
//  printf("Acknowledgement recieved, pru1 halted.\n");

  /* clear the event (if asserted) */
  if(prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT)) {
    printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
    rtn = -1;
  }

  /* clear the event (if asserted) */
  if(prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT)) {
    printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
    rtn = -1;
  }

  sleep(1);

  /* halt and disable the PRU (if running) */
  if( (rtn = prussdrv_pru_disable(PRU_SENSOR)) != 0) {
    printf("prussdrv_pru_disable() failed with %i\n", rtn);
    rtn = -1;
   }

  /* halt and disable the PRU (if running) */
  if( (rtn = prussdrv_pru_disable(PRU_CONTROL)) != 0) {
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

/* ----------------------------------------------------------------------------
 * Function: int pru_mem_init(void)
 *
 * This function sets memory pointers to pru0RAM, pru1RAM, sharedRAM
 *
 * Outputs:  returns 0 on success
 * ------------------------------------------------------------------------- */
int pru_mem_init(void)
{
  int rtn = 0;
  void *ptr = NULL;

  /* Memory Map for params (pru0 DRAM)*/
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  param = (param_mem_t *) ptr;

  /* Memory Map for feedforward lookup table (pru1 DRAM) */
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  lookUp = (lookUp_mem_t *) ptr;

  /* Memory Map for shared memory */
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  p = (shared_mem_t *) ptr;

  printf("Memory Allocation:\n");
  printf("\tSize of parameter memory (pru0 DRAM): %i bytes.\n",
          sizeof(*param));
  printf("\tSize of feedforward lookup table (pru1 DRAM): %i bytes.\n",
          sizeof(*lookUp));
  printf("\tSize of shared memory (SRAM): %i bytes.\n",
          sizeof(*p));

  /* Zero State Buffers */
  for(int i=0; i<NUM_OF_BUFFS; i++){
    for(int j=0; j<SIZE_OF_BUFFS; j++){
      p->state[i][j].timeStamp = 0;
      p->state[i][j].avgPeriod = 0;
      p->state[i][j].anklePos = 0;
      p->state[i][j].ankleVel = 0;
      p->state[i][j].gaitPhase = 0;
      p->state[i][j].motorDuty = 0;

      for(int k=0; k<NUM_ADC; k++){
        p->state[i][j].adc[k] = 0;
      }

      for(int k=0; k<NUM_IMU; k++){
        p->state[i][j].imu[k] = 0;
      }
    }
  }
  return 0;
}


/* ----------------------------------------------------------------------------
 * Function: int armToPru0Interrupt(void)
 *
 * This function sends an interrupt to pru0 from the arm.
 *
 * Outputs:  returns 0 on success
 * ------------------------------------------------------------------------- */
int armToPru0Interrupt(void)
{
  int rtn = 0;

  if( (rtn = prussdrv_pru_send_event(ARM_PRU0_INTERRUPT)) != 0){
    printf("prussdrv_pru_send_event() failed with %i\n", rtn);
    return -1;
  }
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function: int armToPru1Interrupt(void)
 *
 * This function sends an interrupt to pru1 from the arm.
 *
 * Outputs:  returns 0 on success
 * ------------------------------------------------------------------------- */
int armToPru1Interrupt(void)
{
  int rtn = 0;

  if( (rtn = prussdrv_pru_send_event(ARM_PRU1_INTERRUPT)) != 0){
    printf("prussdrv_pru_send_event() failed with %i\n", rtn);
    return -1;
  }
  return 0;
}

void writeState(uint8_t bi)
{

  if(debug){
    printf("%i\t", p->state[bi][0].timeStamp);
    printf("%i\t", p->state[bi][0].avgPeriod);
    printf("%i\t", p->state[bi][0].gaitPhase);
    printf("%i\t", p->state[bi][0].anklePos);
    printf("%i\t", p->state[bi][0].ankleVel);
    printf("%i\t", p->state[bi][0].motorDuty);
    printf("%i\t", p->state[bi][0].adc[0]);
    printf("%i\t", p->state[bi][0].adc[1]);
    printf("%i\t", p->state[bi][0].adc[2]);
    printf("%i\t", p->state[bi][0].adc[3]);
    printf("%i\t", p->state[bi][0].adc[4]);
    printf("%i\t", p->state[bi][0].adc[5]);
    printf("%i\t", p->state[bi][0].adc[6]);
    printf("%i\t", p->state[bi][0].adc[7]);
    printf("%i\t", p->state[bi][0].imu[0]);
    printf("%i\t", p->state[bi][0].imu[1]);
    printf("%i\t", p->state[bi][0].imu[2]);
    printf("%i\t", p->state[bi][0].imu[3]);
    printf("%i\t", p->state[bi][0].imu[4]);
    printf("%i", p->state[bi][0].imu[5]);
    printf("\n");
  }
  else {
    for(int i=0; i<SIZE_OF_BUFFS; i++){
      fprintf(fid,"%i\t", p->state[bi][i].timeStamp);
      fprintf(fid,"%i\t", p->state[bi][i].avgPeriod);
      fprintf(fid,"%i\t", p->state[bi][i].gaitPhase);
      fprintf(fid,"%i\t", p->state[bi][i].anklePos);
      fprintf(fid,"%i\t", p->state[bi][i].ankleVel);
      fprintf(fid,"%i\t", p->state[bi][i].motorDuty);
      fprintf(fid,"%i\t", p->state[bi][i].adc[0]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[1]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[2]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[3]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[4]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[5]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[6]);
      fprintf(fid,"%i\t", p->state[bi][i].adc[7]);
      fprintf(fid,"%i\t", p->state[bi][i].imu[0]);
      fprintf(fid,"%i\t", p->state[bi][i].imu[1]);
      fprintf(fid,"%i\t", p->state[bi][i].imu[2]);
      fprintf(fid,"%i\t", p->state[bi][i].imu[3]);
      fprintf(fid,"%i\t", p->state[bi][i].imu[4]);
      fprintf(fid,"%i", p->state[bi][i].imu[5]);
      fprintf(fid,"\n");

      /* Zero State */
      p->state[bi][i].timeStamp = 0;
      p->state[bi][i].avgPeriod = 0;
      p->state[bi][i].gaitPhase = 0;
      p->state[bi][i].anklePos = 0;
      p->state[bi][i].ankleVel = 0;
      p->state[bi][i].motorDuty = 0;
      for(int k=0; k<NUM_ADC; k++){
        p->state[bi][i].adc[k] = 0;
      }
      for(int k=0; k<NUM_IMU; k++){
        p->state[bi][i].imu[k] = 0;
      }
    }
  }
}

void clearFlowBitFeild(void)
{
  p->cntrl = 0x00;
}

void enable(void)
{
  p->cntrl_bit.enable = 1;
}

void disable(void)
{
  p->cntrl_bit.enable = 0;
}

int isBuffer0Full(void)
{
  return (int)p->cntrl_bit.buffer0_full;
}

int isBuffer1Full(void)
{
  return (int)p->cntrl_bit.buffer1_full;
}

void resetBuffer0FullFlag(void)
{
  p->cntrl_bit.buffer0_full = 0;
}

void resetBuffer1FullFlag(void)
{
  p->cntrl_bit.buffer1_full = 0;
}


/* ----------------------------------------------------------------------------
 *
 * This function converts freq in Hz to number of pru clock ticks.
 *
 * Inputs:    freq_hz   -   freq. in hz (float)
 *
 * Outputs:   pruTicks  -   pru clock ticks (uint32_t)
 * ------------------------------------------------------------------------- */
uint32_t hzToPruTicks(float freq_hz)
{
  return (uint32_t)( (float)PRU_CLK / freq_hz);
}

/* ----------------------------------------------------------------------------
 * Function: float pruTicksToHz(uint32_t ticks)
 *
 * This function converts pru ticks to freq in Hz.
 *
 * Inputs:    ticks     -   pru clock ticks (uint32_t)
 *
 * Outputs:   freq_hz   -   freq_hz (float)
 * ------------------------------------------------------------------------- */
float pruTicksToHz(uint32_t ticks)
{
  return 1/((float)ticks/200000000.0);
}

void setKp(float Kp)
{
  param->Kp = (uint16_t)Kp;
}

uint16_t getKp(void)
{
  return param->Kp;
}

void setKd(float Kd)
{
  param->Kd = (uint16_t)Kd;
}

uint16_t getKd(void)
{
  return (float)param->Kd;
}

void setAnklePos0(float pos0)
{
  param->anklePos0 = (int16_t)pos0;
}

int16_t getAnklePos0(void)
{
  return param->anklePos0;
}

int logFileInit(char* fileName)
{
  char timestr[256];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  /* Create log file */
  strftime(timestr, sizeof(timestr)-1, "%Y_%m_%d_%H-%M", t);
  fid = fopen(fileName, "w");

  /* Create header */
  fprintf(fid,"%% anklebot \n");
  fprintf(fid,"%% Date: %s\n", timestr);
  fprintf(fid,"%% Sample frequency = %u\n", param->frq_hz);
  return 0;
}

void saveParameters(char *file)
{
  FILE *f = fopen(file, "w");
  if(f != NULL){
    fprintf(f, "%u\t// Freq.\n", param->frq_hz);
    fprintf(f,"%u\t// Freq. Ticks\n", 0);
    fprintf(f,"%hu\t// Subject Mass\n", param->mass);
    fprintf(f,"%hu\t// toe_hs threshold\n", param->gp_toe_hs);
    fprintf(f,"%hu\t// mid_hs threshold\n", param->gp_mid_hs);
    fprintf(f,"%hu\t// heel_hs threshold\n", param->gp_heel_hs);
    fprintf(f,"%hu\t// toe_to threshold\n", param->gp_toe_to);
    fprintf(f,"%hu\t// mid_to threshold\n", param->gp_mid_to);
    fprintf(f,"%hu\t// heel_to threshold\n", param->gp_heel_to);
    fprintf(f,"%hu\t// gpOnLeftFoot\n", param->gpOnLeftFoot);
    fprintf(f,"%hu\t// Kp\n", param->Kp);
    fprintf(f,"%hu\t// Kd\n", param->Kd);
    fprintf(f,"%hd\t// anklePos0\n", param->anklePos0);
    fclose(f);
  }
  else{
    printf("File doesn't exsist.\n");
  }
}

void loadParameters(char *file)
{
  FILE *f = fopen(file, "r");
  if(f != NULL){
    fscanf(f,"%u%*[^\n]\n", &param->frq_hz);
    fscanf(f,"%u%*[^\n]\n", &param->frq_clock_ticks);
    fscanf(f,"%hu%*[^\n]\n", &param->mass);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_toe_hs);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_mid_hs);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_heel_hs);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_toe_to);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_mid_to);
    fscanf(f,"%hu%*[^\n]\n", &param->gp_heel_to);
    fscanf(f,"%hu%*[^\n]\n", &param->gpOnLeftFoot);
    fscanf(f,"%hu%*[^\n]\n", &param->Kp);
    fscanf(f,"%hu%*[^\n]\n", &param->Kd);
    fscanf(f,"%hd%*[^\n]\n", &param->anklePos0);
    fclose(f);

    param->frq_clock_ticks = hzToPruTicks(param->frq_hz);
  }
  else {
    printf("File doesn't exsist!");
  }
}

void printParameters(void)
{
  printf("Parameters:\n");
  printf("\t Frq = %i (Hz)\n", param->frq_hz);
  printf("\t Ticks = %i\n", param->frq_clock_ticks);
  printf("\t gp_toe_hs = %i\n", param->gp_toe_hs);
  printf("\t gp_mid_hs = %i\n", param->gp_mid_hs);
  printf("\t gp_heel_hs = %i\n", param->gp_heel_hs);
  printf("\t gp_toe_to = %i\n", param->gp_toe_to);
  printf("\t gp_mid_to = %i\n", param->gp_mid_to);
  printf("\t gp_heel_to = %i\n", param->gp_heel_to);
  printf("\t gpOnLeftFoot = %i\n", param->gpOnLeftFoot);
  printf("\t Kp = %i\n", param->Kp);
  printf("\t Kd = %i\n", param->Kd);
  printf("\t anklePos0 = %i\n", param->anklePos0);
}

void loadLookUpTable(char *file)
{
  FILE *f = fopen(file, "r");
  float value;

  if(f != NULL){
    for(int i=0; i<NUM_FF_LT; i++){
      fscanf(f,"%f\n", &value);
      lookUp->ff_ankleTorque[i] = (int16_t) (value*((float)param->mass)*100.0);
    }
    fclose(f);
  }
  else {
    printf("File doesn't exsist!");
  }

}



void closeLogFile(void)
{
  fclose(fid);
}

void clearBufferFlags(void)
{
  p->cntrl_bit.buffer0_full = 0;
  p->cntrl_bit.buffer1_full = 0;
}

void setTareEncoderBit(void)
{
  p->cntrl_bit.encoderTare = 1;
}





