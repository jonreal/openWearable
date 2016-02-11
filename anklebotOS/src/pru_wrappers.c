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
#include "filter.h"

#define M_PI (3.14159265359)

#define AM33XX

/* Pointer to state data */
shared_mem_t *s;

/* Pointer to sensor pru params */
param_mem_t *p;

/* Pointer to controller pru params */
lookUp_mem_t *l;


void initDebugBuffer(void)
{
  for(int i=0; i<10; i++){
    p->debugBuffer[i] = 0;
  }
}

void printDebugBuffer(void)
{
  printf("\n\n---- Debug Buffer ----\n");
  for(int i=0; i<10; i++){
    printf("0x%X \t%i\n", (uint32_t)p->debugBuffer[i], p->debugBuffer[i]);
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

  // Load and run PRU program
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

  // Initialize PRUs
  if( (rtn = prussdrv_init()) != 0){
    printf("prusdrv_init() failed with %i.\n", rtn);
    return rtn;
  }

  // Open interrupt PRU0
  if( (rtn = prussdrv_open(PRU_EVTOUT_0)) != 0){
    printf("prussdrv_open() failed with %i.\n", rtn);
    return rtn;
  }

  // Open interrupt PRU1
  if( (rtn = prussdrv_open(PRU_EVTOUT_1)) != 0){
    printf("prussdrv_open() failed with %i.\n", rtn);
    return rtn;
  }

  // Initialize Interrupt
  if( (rtn = prussdrv_pruintc_init(&intc)) != 0){
    printf("prussdrv_pruintc_init() failed with %i.\n", rtn);
    return rtn;
  }

  // Initialize memory
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

  // clear the event (if asserted)
  if(prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT)) {
    printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
    rtn = -1;
  }

  // clear the event (if asserted)
  if(prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT)) {
    printf("prussdrv_pru_clear_event() failed with %i\n", rtn);
    rtn = -1;
  }

  sleep(1);

  // halt and disable the PRU (if running)
  if( (rtn = prussdrv_pru_disable(PRU0)) != 0) {
    printf("prussdrv_pru_disable() failed with %i\n", rtn);
    rtn = -1;
   }

  // halt and disable the PRU (if running)
  if( (rtn = prussdrv_pru_disable(PRU1)) != 0) {
    printf("prussdrv_pru_disable() failed with %i\n", rtn);
    rtn = -1;
   }

  // release the PRU clocks and disable prussdrv module
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

  // Memory Map for params (pru0 DRAM)
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  p = (param_mem_t *) ptr;


  // Memory Map for feedforward lookup table (pru1 DRAM)
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  l = (lookUp_mem_t *) ptr;

  // Memory Map for shared memory
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  s = (shared_mem_t *) ptr;

  printf("Memory Allocation:\n");
  printf("\tSize of parameter memory (pru0 DRAM): %i bytes.\n",
          sizeof(*p));
  printf("\tSize of feedforward lookup table (pru1 DRAM): %i bytes.\n",
          sizeof(*l));
  printf("\tSize of shared memory (SRAM): %i bytes.\n",
          sizeof(*s));

  /* Zero State */
//  for(int i=0; i<NUM_OF_BUFFS; i++){
//    for(int j=0; j<SIZE_OF_BUFFS; j++){
//      s->state[i][j].timeStamp = 0;
//      s->state[i][j].sync = 0;
//      s->state[i][j].avgPeriod = 0;
//      s->state[i][j].heelStrikeCnt = 0;
//      s->state[i][j].gaitPhase = 0;
//      s->state[i][j].anklePos = 0;
//      s->state[i][j].ankleVel = 0;
//      s->state[i][j].fbCurrentCmd = 0;
//      s->state[i][j].ffCurrentCmd = 0;
//      s->state[i][j].motorDuty = 0;
//      for(int k=0; k<NUM_ADC; k++){
//        s->state[i][j].adc[k] = 0;
//      }
//      for(int k=0; k<NUM_IMU; k++){
//        s->state[i][j].imu[k] = 0;
//      }
//    }
//  }
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

/* ----------------------------------------------------------------------------
 * Function: void writeState(uint32_t bi)
 *
 * This function writes state (data) to file. If debug mode is on, this
 * prints low fidelity data to screen.
 *
 * Outputs: none
 *
 * Inputs: uint32_t bi - buffer index
 * ------------------------------------------------------------------------- */
void writeState(uint8_t bi)
{

//  if(debug){
//    printf("%i\t", s->state[bi][0].timeStamp);
//    printf("%i\t", s->state[bi][0].sync);
//    printf("%i\t", s->state[bi][0].avgPeriod);
//    printf("%i\t", s->state[bi][0].heelStrikeCnt);
//    printf("%i\t", s->state[bi][0].gaitPhase);
//    printf("%i\t", s->state[bi][0].anklePos);
//    printf("%i\t", s->state[bi][0].ankleVel);
//    printf("%i\t", s->state[bi][0].fbCurrentCmd);
//    printf("%i\t", s->state[bi][0].ffCurrentCmd);
//    printf("%i\t", s->state[bi][0].motorDuty);
//    printf("%i\t", s->state[bi][0].adc[0]);
//    printf("%i\t", s->state[bi][0].adc[1]);
//    printf("%i\t", s->state[bi][0].adc[2]);
//    printf("%i\t", s->state[bi][0].adc[3]);
//    printf("%i\t", s->state[bi][0].adc[4]);
//    printf("%i\t", s->state[bi][0].adc[5]);
//    printf("%i\t", s->state[bi][0].adc[6]);
//    printf("%i\t", s->state[bi][0].adc[7]);
//    printf("%i\t", s->state[bi][0].imu[0]);
//    printf("%i\t", s->state[bi][0].imu[1]);
//    printf("%i\t", s->state[bi][0].imu[2]);
//    printf("%i\t", s->state[bi][0].imu[3]);
//    printf("%i\t", s->state[bi][0].imu[4]);
//    printf("%i", s->state[bi][0].imu[5]);
//    printf("\n");
//  }
//  else {
//    for(int i=0; i<SIZE_OF_BUFFS; i++){
//      fprintf(fid,"%i\t", s->state[bi][i].timeStamp);
//      fprintf(fid,"%i\t", s->state[bi][i].sync);
//      fprintf(fid,"%i\t", s->state[bi][i].avgPeriod);
//      fprintf(fid,"%i\t", s->state[bi][i].heelStrikeCnt);
//      fprintf(fid,"%i\t", s->state[bi][i].gaitPhase);
//      fprintf(fid,"%i\t", s->state[bi][i].anklePos);
//      fprintf(fid,"%i\t", s->state[bi][i].ankleVel);
//      fprintf(fid,"%i\t", s->state[bi][i].fbCurrentCmd);
//      fprintf(fid,"%i\t", s->state[bi][i].ffCurrentCmd);
//      fprintf(fid,"%i\t", s->state[bi][i].motorDuty);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[0]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[1]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[2]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[3]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[4]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[5]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[6]);
//      fprintf(fid,"%i\t", s->state[bi][i].adc[7]);
//      fprintf(fid,"%i\t", s->state[bi][i].imu[0]);
//      fprintf(fid,"%i\t", s->state[bi][i].imu[1]);
//      fprintf(fid,"%i\t", s->state[bi][i].imu[2]);
//      fprintf(fid,"%i\t", s->state[bi][i].imu[3]);
//      fprintf(fid,"%i\t", s->state[bi][i].imu[4]);
//      fprintf(fid,"%i", s->state[bi][i].imu[5]);
//      fprintf(fid,"\n");
//
//      /* Zero State */
//      s->state[bi][i].timeStamp = 0;
//      s->state[bi][i].sync = 0;
//      s->state[bi][i].avgPeriod = 0;
//      s->state[bi][i].heelStrikeCnt = 0;
//      s->state[bi][i].gaitPhase = 0;
//      s->state[bi][i].anklePos = 0;
//      s->state[bi][i].ankleVel = 0;
//      s->state[bi][i].fbCurrentCmd = 0;
//      s->state[bi][i].ffCurrentCmd = 0;
//      s->state[bi][i].motorDuty = 0;
//      for(int k=0; k<NUM_ADC; k++){
//        s->state[bi][i].adc[k] = 0;
//      }
//      for(int k=0; k<NUM_IMU; k++){
//        s->state[bi][i].imu[k] = 0;
//      }
//    }
//  }
}

/* ----------------------------------------------------------------------------
 * Function: void clearFlowBitFeild(void)
 *
 * This function clears flow bit feild used by prus.
 * ------------------------------------------------------------------------- */
void clearFlowBitFeild(void)
{
  s->cntrl = 0x00;
}

/* ----------------------------------------------------------------------------
 * Function: void enable(void)
 *
 * This function sets the enbable bit in the flow bit feild to 1 (enable).
 * ------------------------------------------------------------------------- */
void enablePru(int en)
{
  if(en == 1)
    s->cntrl_bit.enable = 1;
  else
    s->cntrl_bit.enable = 0;
}

/* ----------------------------------------------------------------------------
 * Function: void isBuffer0Full(void)
 *
 * This function returns 1 if buffer0 is full.
 *
 * Output: 1 - buffer0 full
 *         0 - buffer0 not full
 * ------------------------------------------------------------------------- */
int buffer0Full(void)
{
  return (int)s->cntrl_bit.buffer0_full;
}


int buffer1Full(void)
{
  return (int)s->cntrl_bit.buffer1_full;
}

/* ----------------------------------------------------------------------------
 * Function: void resetBuffer0FullFlag(void)
 *
 * This function clears bufferFull flag.
 * ------------------------------------------------------------------------- */
void clearBuffer0FullFlag(void)
{
  s->cntrl_bit.buffer0_full = 0;
}

void clearBuffer1FullFlag(void)
{
  s->cntrl_bit.buffer1_full = 0;
}

void clearBufferFlags(void)
{
  s->cntrl_bit.buffer0_full = 0;
  s->cntrl_bit.buffer1_full = 0;
}

/* ----------------------------------------------------------------------------
 * Function: uint32_t hzToPruTicks(float freq_hz)
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

/* ----------------------------------------------------------------------------
 * Functions: void setN(float N)
 *
 * These function set params - where N is the param.
 * ------------------------------------------------------------------------- */
void setKp(float newKp)
{
  p->Kp = (fix16_t) fix16_from_float(newKp);
}

void setKd(float newKd)
{
  p->Kd = (fix16_t) fix16_from_float(newKd);
}

void setAnklePos0(float newAnklePos0)
{
  p->anklePos0 = (fix16_t) fix16_from_float(newAnklePos0);
}

/* ----------------------------------------------------------------------------
 * Functions: uint16_t getN(float N)
 *
 * These functions return param values - where N is the param.
 * ------------------------------------------------------------------------- */
float getKp(void)
{
  return fix16_to_float(p->Kp);
}

float getKd(void)
{
  return fix16_to_float(p->Kd);
}

float getAnklePos0(void)
{
  return fix16_to_float(p->anklePos0);
}

/* ----------------------------------------------------------------------------
 * Functions: int logFileInit(char* fileName)
 *
 * This function creates a log file.
 *
 * TODO: check if file exists.
 * ------------------------------------------------------------------------- */
int logFileInit(char* fileName)
{
  char timestr[256];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  /* Create log file */
  strftime(timestr, sizeof(timestr)-1, "%d-%b-%Y %X", t);
  flog = fopen(fileName, "w");

  /* Create header */
  fprintf(flog, "%% anklebot \n");
  fprintf(flog, "%% Date: %s\n", timestr);
  fprintf(flog, "%% Sample frequency = %u\n", p->frq_hz);

  //TODO add paramters and variable headings
  return 0;
}

/* ----------------------------------------------------------------------------
 * Functions: void saveParameters(char* file)
 *
 * This function saves current parameters to file.
 * ------------------------------------------------------------------------- */
void saveParameters(char* file)
{
  FILE* fp = fopen(file, "w");
  if(fp != NULL){
    fprintf(fp, "%u\t// Freq.\n", p->frq_hz);
    fprintf(fp, "%u\t// Freq. Ticks\n", 0);
    fprintf(fp, "%hu\t// Subject Mass\n", p->mass);
    fprintf(fp, "%hu\t// Kp\n", p->Kp);
    fprintf(fp, "%hu\t// Kd\n", p->Kd);
    fprintf(fp, "%hd\t// anklePos0\n", p->anklePos0);
    fclose(fp);
  }
  else{
    printf("\nParameter file doesn't exsist.\n");
  }
}

/* ----------------------------------------------------------------------------
 * Functions: void loadParameters(char* file)
 *
 * This function loads parameters from file.
 * ------------------------------------------------------------------------- */
int loadParameters(char *file)
{
  FILE* fp = fopen(file, "r");
  float t1;

  if(fp != NULL){
    fscanf(fp, "%u%*[^\n]\n", &p->frq_hz);
    fscanf(fp, "%u%*[^\n]\n", &p->frq_clock_ticks);
    fscanf(fp, "%u%*[^\n]\n", &p->mass);

    fscanf(fp, "%f%*[^\n]\n", &t1);
    p->Kp = fix16_from_float(t1);

    fscanf(fp, "%f%*[^\n]\n", &t1);
    p->Kd = fix16_from_float(t1);

    fscanf(fp, "%f%*[^\n]\n", &t1);
    p->anklePos0 = fix16_from_float(t1);

    fclose(fp);
    p->frq_clock_ticks = hzToPruTicks(p->frq_hz);

    return 0;
  }
  return -1;
}

/* ----------------------------------------------------------------------------
 * Functions: void printParameters(void)
 *
 * This function prints current parameters.
 * ------------------------------------------------------------------------- */
void printParameters(FILE *fp)
{
  fprintf(fp, "Parameters:\n");
  fprintf(fp, "\t Frq = %i (Hz)\n", p->frq_hz);
  fprintf(fp, "\t Ticks = %i\n", p->frq_clock_ticks);
  fprintf(fp, "\t Kp = %f\n", fix16_to_float(p->Kp));
  fprintf(fp, "\t Kd = %f\n", fix16_to_float(p->Kd));
  fprintf(fp, "\t anklePos0 = %f\n", fix16_to_float(p->anklePos0));
}

/* ----------------------------------------------------------------------------
 * Functions: void loadLookUpTable(char* file)
 *
 * This function loads lookup table (Feedforward) from file to memory.
 * ------------------------------------------------------------------------- */
int loadLookUpTable(char* file)
{
  FILE* fp = fopen(file, "r");
  float value;

  if(fp != NULL){
    for(int i=0; i<NUM_FF_LT; i++){
      fscanf(fp, "%f\n", &value);
      l->uff[i] = (fix16_t) fix16_from_float(value * (float)p->mass);
    }
    fclose(fp);
    return 0;
  }
  return -1;
}

/* ----------------------------------------------------------------------------
 * Functions: void loadFilterCoeff(char* file)
 *
 * This function loads filter coeff from file to memory.
 *
 * file format:
 *    FilterOrder
 *    b(1)
 *    .
 *    .
 *    .
 *    b(FilterOrder+1)
 *    a(1)
 *    .
 *    .
 *    .
 *    a(FilterOrder+1)
 *
 * NOTE: Coefficients must be represted as floating point
 * ------------------------------------------------------------------------- */
int loadIirFilterCoeff(char *file)
{
  FILE* fp = fopen(file, "r");
  float v;

  if(fp != NULL){

    // First element is filter order
    fscanf(fp, "%u\n", &p->filt.N);

    // Numerator coefficients, b
    for(int i=0; i<p->filt.N+1; i++){
      fscanf(fp,"%f\n", &v);
      p->filt.b[i] = fix16_from_float(v);
    }

    // Denominator coefficients, a
    for(int i=0; i<p->filt.N+1; i++){
      fscanf(fp,"%f\n", &v);
      p->filt.a[i] = fix16_from_float(v);
    }
    fclose(fp);
    return 0;
  }
  return -1;
}
void printFirCoeff(FILE *fp)
{
  for(int i=0; i<p->filt.N+1; i++){
    fprintf(fp, "\tb[%i] : %8.8f\ta[%i] : %8.8f\n",
            i, fix16_to_float(p->filt.b[i]),
            i, fix16_to_float(p->filt.a[i]));
  }
}
void printFFLookUpTable(FILE *fp)
{
  for(int i=0; i<NUM_FF_LT; i++){
    fprintf(fp, "\t%i\t%f\n", i, fix16_to_float(l->uff[i]));
  }
}

void enableFF(int en)
{
  if(en == 1)
    s->cntrl_bit.doFeedForward = 1;
  else
    s->cntrl_bit.doFeedForward = 0;
}

void resetGaitPhase(void)
{
  s->cntrl_bit.resetGaitPhase = 1;
}

int FFenabled(void)
{
  return (s->cntrl_bit.doFeedForward);
}

void testFF(void)
{
  s->cntrl_bit.testFF = 1;
}

void stopTestFF(void)
{
  s->cntrl_bit.testFF = 0;
}

void closeLogFile(void)
{
  fclose(flog);
}

void setTareEncoderBit(void)
{
  s->cntrl_bit.encoderTare = 1;
}





