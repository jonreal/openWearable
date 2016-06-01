#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "mem_types.h"
#include "pru_wrappers.h"
#include "fix16.h"


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
    printf("0x%X \t%i\n", p->debugBuffer[i], p->debugBuffer[i]);
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

  printMemoryAllocation(stdout);

  for(int i=0; i<NUM_OF_BUFFS; i++)
    for(int j=0; j<SIZE_OF_BUFFS; j++)
      zeroState(i,j);

  return 0;
}

void printMemoryAllocation(FILE *fp)
{
  fprintf(fp, "\n#Memory Allocation:\n"
              "#\tParameter memory: %i bytes.\n"
              "#\tLookup table memory: %i bytes.\n"
              "#\tData memory: %i bytes.\n#",
              sizeof(*p), sizeof(*l), sizeof(*s));
  fflush(fp);
}

void sprintMemoryAllocation(char* buffer)
{
  sprintf(buffer,"#\n#Memory Allocation:\n"
              "#\tParameter memory: %i bytes.\n"
              "#\tLookup table memory: %i bytes.\n"
              "#\tData memory: %i bytes.\n#",
              sizeof(*p), sizeof(*l), sizeof(*s));
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

void zeroState(uint8_t bi, uint8_t si)
{
  s->state[bi][si].timeStamp = 0;
  s->state[bi][si].sync = 0;
  s->state[bi][si].r_hsStamp = 0;
  s->state[bi][si].l_hsStamp = 0;
  s->state[bi][si].r_meanGaitPeriod = 0;
  s->state[bi][si].l_meanGaitPeriod = 0;
  s->state[bi][si].r_percentGait = 0;
  s->state[bi][si].l_percentGait = 0;
  s->state[bi][si].r_gaitPhase = 0;
  s->state[bi][si].l_gaitPhase = 0;
  s->state[bi][si].motorDuty = 0;
  s->state[bi][si].anklePos = 0;
  s->state[bi][si].ankleVel = 0;
  s->state[bi][si].u_fb = 0;
  s->state[bi][si].u_ff = 0;
  s->state[bi][si].adc[0] = 0;
  s->state[bi][si].adc[1] = 0;
  s->state[bi][si].adc[2] = 0;
  s->state[bi][si].adc[3] = 0;
  s->state[bi][si].adc[4] = 0;
  s->state[bi][si].adc[5] = 0;
  s->state[bi][si].adc[6] = 0;
  s->state[bi][si].adc[7] = 0;
///  s->state[bi][si].imu[0] = 0;
///  s->state[bi][si].imu[1] = 0;
///  s->state[bi][si].imu[2] = 0;
///  s->state[bi][si].imu[3] = 0;
///  s->state[bi][si].imu[4] = 0;
///  s->state[bi][si].imu[5] = 0;

}
void printStateHeader(FILE *fp)
{
  fprintf(fp,
          "\n# frame\t"
          "sync\t"
          "r_hs\t"
          "l_hs\t"
          "r_Tp\t"
          "l_Tp\t"
          "r_Pgait\t"
          "l_Pgait\t"
          "r_gp\t"
          "l_gp\t"
          "duty\t"
          "ankPos\t"
          "ankVel\t"
          "u_fb\t"
          "u_ff\t"
          "mtrCurr\t"
          "mtrVel\t"
          "l_s1\t"
          "l_s2\t"
          "l_s3\t"
          "r_s1\t"
          "r_s2\t"
          "r_s3\t"
          "l_d_s3\t"
          "r_d_s3\t"
//          "imu0\t"
//          "imu1\t"
//          "imu2\t"
//          "imu3\t"
//          "imu4\t"
//          "imu5"
          "\n");
  fflush(fp);
}
void sprintStateHeader(char* buffer)
{
  sprintf(buffer,
          "\n# frame\t"
          "sync\t"
          "r_hs\t"
          "l_hs\t"
          "r_Tp\t"
          "l_Tp\t"
          "r_Pgait\t"
          "l_Pgait\t"
          "r_gp\t"
          "l_gp\t"
          "duty\t"
          "ankPos\t"
          "ankVel\t"
          "u_fb\t"
          "u_ff\t"
          "mtrCurr\t"
          "mtrVel\t"
          "l_s1\t"
          "l_s2\t"
          "l_s3\t"
          "r_s1\t"
          "r_s2\t"
          "r_s3\t"
          "l_d_s3\t"
          "r_d_s3\t"
//          "imu0\t"
//          "imu1\t"
//          "imu2\t"
//          "imu3\t"
//          "imu4\t"
//          "imu5"
          "\n");
}

void printState(uint8_t bi, uint8_t si, FILE *fp)
{

  fflush(fp);
  fprintf(fp,
          "%u\t"    // timeStamp - uint32_t
          "%u\t"    // sync - uint16_t
          "%u\t"    // r_hsStamp - uint32_t
          "%u\t"    // l_hsStamp - uint32_t
          "%u\t"    // r_meanGaitPeriod - uint16_t
          "%u\t"    // l_meanGaitPeriod - uint16_t
          "%u\t"    // r_percentGait - uint16_t
          "%u\t"    // l_percentGait - uint16_t
          "%u\t"    // r_gaitPhase - uint16_t
          "%u\t"    // l_gaitPhase - uint16_t
          "%i\t"    // motorDuty - int16_t
          "%.5f\t"  // anklePos - fix16_t (convert to float)
          "%.5f\t"  // ankleVel - fix16_t (convert to float)
          "%.5f\t"  // u_fb - fix16_t (convert to float)
          "%.5f\t"  // u_ff - fix16_t (convert to float)
          "%i\t"    // adc[0] (motor current) - int16_t
          "%i\t"    // adc[1] (motor vel) - int16_t
          "%i\t"    // adc[2] (amp1s1) - int16_t
          "%i\t"    // adc[3] (amp1s2) - int16_t
          "%i\t"    // adc[4] (amp1s3) - int16_t
          "%i\t"    // adc[5] (amp2s1) - int16_t
          "%i\t"    // adc[6] (amp2s2) - int16_t
          "%i\t"    // adc[7] (amp2s3) - int16_t
          "%i\t"    // heelVel - int16_t
          "%i\t"    // heelVel - int16_t
//          "%i\t"    // imu[0] - int16_t
//          "%i\t"    // imu[1] - int16_t
//          "%i\t"    // imu[2] - int16_t
//          "%i\t"    // imu[3] - int16_t
//          "%i\t"    // imu[4] - int16_t
//          "%i\t"    // imu[5] - int16_t
          "\n", s->state[bi][si].timeStamp,
                s->state[bi][si].sync,
                s->state[bi][si].r_hsStamp,
                s->state[bi][si].l_hsStamp,
                s->state[bi][si].r_meanGaitPeriod,
                s->state[bi][si].l_meanGaitPeriod,
                s->state[bi][si].r_percentGait,
                s->state[bi][si].l_percentGait,
                s->state[bi][si].r_gaitPhase,
                s->state[bi][si].l_gaitPhase,
                s->state[bi][si].motorDuty,
                fix16_to_float(s->state[bi][si].anklePos),
                fix16_to_float(s->state[bi][si].ankleVel),
                fix16_to_float(s->state[bi][si].u_fb),
                fix16_to_float(s->state[bi][si].u_ff),
                s->state[bi][si].adc[0],
                s->state[bi][si].adc[1],
                s->state[bi][si].adc[2],
                s->state[bi][si].adc[3],
                s->state[bi][si].adc[4],
                s->state[bi][si].adc[5],
                s->state[bi][si].adc[6],
                s->state[bi][si].adc[7],
                s->state[bi][si].d_heelForce[0],
                s->state[bi][si].d_heelForce[1]
//                s->state[bi][si].imu[0],
//                s->state[bi][si].imu[1],
//                s->state[bi][si].imu[2],
//                s->state[bi][si].imu[3],
//                s->state[bi][si].imu[4],
//                s->state[bi][si].imu[5]
               );

  fflush(fp);
}

void sprintState(uint8_t bi, uint8_t si, char* buffer)
{
  sprintf(buffer,
          "%u\t"    // timeStamp - uint32_t
          "%u\t"    // sync - uint16_t
          "%u\t"    // r_hsStamp - uint32_t
          "%u\t"    // l_hsStamp - uint32_t
          "%u\t"    // r_meanGaitPeriod - uint16_t
          "%u\t"    // l_meanGaitPeriod - uint16_t
          "%u\t"    // r_percentGait - uint16_t
          "%u\t"    // l_percentGait - uint16_t
          "%u\t"    // r_gaitPhase - uint16_t
          "%u\t"    // l_gaitPhase - uint16_t
          "%i\t"    // motorDuty - int16_t
          "%.5f\t"  // anklePos - fix16_t (convert to float)
          "%.5f\t"  // ankleVel - fix16_t (convert to float)
          "%.5f\t"  // u_fb - fix16_t (convert to float)
          "%.5f\t"  // u_ff - fix16_t (convert to float)
          "%i\t"    // adc[0] (motor current) - int16_t
          "%i\t"    // adc[1] (motor vel) - int16_t
          "%i\t"    // adc[2] (amp1s1) - int16_t
          "%i\t"    // adc[3] (amp1s2) - int16_t
          "%i\t"    // adc[4] (amp1s3) - int16_t
          "%i\t"    // adc[5] (amp2s1) - int16_t
          "%i\t"    // adc[6] (amp2s2) - int16_t
          "%i\t"    // adc[7] (amp2s3) - int16_t
          "%i\t"    // heelVel - int16_t
          "%i"    // heelVel - int16_t
//          "%i\t"    // imu[0] - int16_t
//          "%i\t"    // imu[1] - int16_t
//          "%i\t"    // imu[2] - int16_t
//          "%i\t"    // imu[3] - int16_t
//          "%i\t"    // imu[4] - int16_t
//          "%i\t"    // imu[5] - int16_t
          "\n", s->state[bi][si].timeStamp,
                s->state[bi][si].sync,
                s->state[bi][si].r_hsStamp,
                s->state[bi][si].l_hsStamp,
                s->state[bi][si].r_meanGaitPeriod,
                s->state[bi][si].l_meanGaitPeriod,
                s->state[bi][si].r_percentGait,
                s->state[bi][si].l_percentGait,
                s->state[bi][si].r_gaitPhase,
                s->state[bi][si].l_gaitPhase,
                s->state[bi][si].motorDuty,
                fix16_to_float(s->state[bi][si].anklePos),
                fix16_to_float(s->state[bi][si].ankleVel),
                fix16_to_float(s->state[bi][si].u_fb),
                fix16_to_float(s->state[bi][si].u_ff),
                s->state[bi][si].adc[0],
                s->state[bi][si].adc[1],
                s->state[bi][si].adc[2],
                s->state[bi][si].adc[3],
                s->state[bi][si].adc[4],
                s->state[bi][si].adc[5],
                s->state[bi][si].adc[6],
                s->state[bi][si].adc[7],
                s->state[bi][si].d_heelForce[0],
                s->state[bi][si].d_heelForce[1]
//                s->state[bi][si].imu[0],
//                s->state[bi][si].imu[1],
//                s->state[bi][si].imu[2],
//                s->state[bi][si].imu[3],
//                s->state[bi][si].imu[4],
//                s->state[bi][si].imu[5]
               );
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
  int len = 0;
  char temp[4096];

  dataLog.writeBuffer[0] = '\0';
  if(debug){
    printState(bi, 0, stdout);
  }
  else
    for(int i=0; i<SIZE_OF_BUFFS; i++){
      sprintState(bi, i, temp);
      strcat(dataLog.writeBuffer, temp);
      zeroState(bi, i);
    }

  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
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
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  int len = 0;
  char timeStr[1024];

  // Init datalog vars.
  dataLog.fd = 0;
  dataLog.location = 0;
  dataLog.addr = NULL;
  dataLog.writeBuffer[0] = '\0';

  // Open file, stretch and write blank
  dataLog.fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC);
  if (lseek(dataLog.fd, LOGSIZE, SEEK_SET) == -1){
    close(dataLog.fd);
    printf("Error stretching file.\n");
    return -1;
  }
  if (write (dataLog.fd, "", 1) == -1){
    close(dataLog.fd);
    printf("Error writing blank at end of file.\n");
    return -1;
  }

  // memory map file
  dataLog.addr = mmap(0, LOGSIZE, PROT_WRITE, MAP_SHARED, dataLog.fd, 0);
  if (dataLog.addr == MAP_FAILED){
    printf("mmap failed\n");
    close(dataLog.fd);
    return -1;
  }

  // Log time
  memcpy(dataLog.writeBuffer, "#Date: ", 7);
  strftime(timeStr, 1024, "%d-%b-%Y %X\n", t);
  strcat(dataLog.writeBuffer, timeStr);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log memory allocation
  sprintMemoryAllocation(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log parameters
  sprintParameters(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log Fir Coeff
  sprintFirCoeff(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

  // Log header
  sprintStateHeader(dataLog.writeBuffer);
  len = strlen(dataLog.writeBuffer);
  memcpy(dataLog.addr + dataLog.location, dataLog.writeBuffer, len);
  dataLog.location += len;
  dataLog.writeBuffer[0] = '\0';

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
    fprintf(fp, "%i\t// Freq.\n", p->frq_hz);
    fprintf(fp, "%i\t// Freq. Ticks\n", 0);
    fprintf(fp, "%i\t// Subject Mass\n", p->mass);
    fprintf(fp, "%.5f\t// Kp\n", fix16_to_float(p->Kp));
    fprintf(fp, "%.5f\t// Kd\n", fix16_to_float(p->Kd));
    fprintf(fp, "%.5f\t// anklePos0\n", fix16_to_float(p->anklePos0));
    fprintf(fp, "%i\t// l_forceThrs\n", p->l_forceThrs);
    fprintf(fp, "%i\t// l_d_forceThrs\n", p->l_d_forceThrs);
    fprintf(fp, "%i\t// r_forceThrs\n", p->r_forceThrs);
    fprintf(fp, "%i\t// l_d_forceThrs\n", p->r_d_forceThrs);
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

    fscanf(fp, "%u%*[^\n]\n", &p->l_forceThrs);
    fscanf(fp, "%u%*[^\n]\n", &p->l_d_forceThrs);
    fscanf(fp, "%u%*[^\n]\n", &p->r_forceThrs);
    fscanf(fp, "%u%*[^\n]\n", &p->r_d_forceThrs);


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
  fprintf(fp, "\n#Parameters:\n"
          "#\tFrq = %i (Hz)\n"
          "#\tTicks = %i\n"
          "#\tKp = %.4f\n"
          "#\tKd = %.4f\n"
          "#\tanklePos0 = %.4f\n"
          "#\tl_forceThrs = %i\n"
          "#\tl_d_forceThrs = %i\n"
          "#\tr_forceThrs = %i\n"
          "#\tr_d_forceThrs = %i\n#",
          p->frq_hz, p->frq_clock_ticks,
          fix16_to_float(p->Kp), fix16_to_float(p->Kd),
          fix16_to_float(p->anklePos0), p->l_forceThrs,
          p->l_d_forceThrs, p->r_forceThrs, p->r_d_forceThrs);
  fflush(fp);
}

void sprintParameters(char* buffer)
{
  sprintf(buffer, "\n#Parameters:\n"
                  "#\tFrq = %i (Hz)\n"
                  "#\tTicks = %i\n"
                  "#\tKp = %.4f\n"
                  "#\tKd = %.4f\n"
                  "#\tanklePos0 = %.4f\n"
                  "#\tl_forceThrs = %i\n"
                  "#\tl_d_forceThrs = %i\n"
                  "#\tr_forceThrs = %i\n"
                  "#\tr_d_forceThrs = %i\n#",
                  p->frq_hz, p->frq_clock_ticks,
                  fix16_to_float(p->Kp), fix16_to_float(p->Kd),
                  fix16_to_float(p->anklePos0), p->l_forceThrs,
                  p->l_d_forceThrs, p->r_forceThrs, p->r_d_forceThrs);
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
      // Scale signal by 1000
      l->u_ff[i] = (int16_t) fix16_to_int(fix16_from_float(value * 1000.0));
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
  fprintf(fp, "\n#Filter Coefficients (N = %i):\n", p->filt.N);
  for(int i=0; i<p->filt.N+1; i++){
    fprintf(fp, "#\tb[%i] : %8.8f\ta[%i] : %8.8f\n",
            i, fix16_to_float(p->filt.b[i]),
            i, fix16_to_float(p->filt.a[i]));
  }
  fprintf(fp, "#");
  fflush(fp);
}

void sprintFirCoeff(char* buffer)
{
  char temp[1024];

  sprintf(buffer, "\n#Filter Coefficients (N = %i):\n", p->filt.N);
  for(int i=0; i<p->filt.N+1; i++){
    sprintf(temp, "#\tb[%i] : %8.8f\ta[%i] : %8.8f\n",
            i, fix16_to_float(p->filt.b[i]),
            i, fix16_to_float(p->filt.a[i]));
    strcat(buffer, temp);
    temp[0] = '\0';
  }
  strcat(buffer, "#");
}

void printFFLookUpTable(FILE *fp)
{
  for(int i=0; i<NUM_FF_LT; i++){
    fprintf(fp, "\t%i\t%i\n", i, l->u_ff[i]);
  }
}

void setFFenable(int en)
{
  if(en == 1){
    s->cntrl_bit.doFeedForward = 1;
    p->FFgain = 0;
  }
  else {
    s->cntrl_bit.doFeedForward = 0;
    p->FFgain = 0;
  }
}

int getFFenable(void)
{
  return (s->cntrl_bit.doFeedForward);
}

float getFFgain(void)
{
  return fix16_to_float(p->FFgain);
}

void setFFgain(float gain)
{
  if (gain > 1.0)
    gain = 1.0;
  p->FFgain = fix16_from_float(gain);
}

void startFFtest(void)
{
  s->cntrl_bit.testFF = 1;
}

void stopFFtest(void)
{
  s->cntrl_bit.testFF = 0;
}


int closeLogFile(void)
{
  // Unmap and truncate file
  if (munmap(dataLog.addr, LOGSIZE) == -1){
    close(dataLog.fd);
    printf("unmap failed\n");
    return -1;
  }
  if (ftruncate(dataLog.fd, dataLog.location) == -1){
    close(dataLog.fd);
    printf("Error truncating file.\n");
    return -1;
  }
  close(dataLog.fd);
  return 0;
}

void setTareEncoderBit(void)
{
  s->cntrl_bit.encoderTare = 1;
}

void setStepCurrent(float cur)
{
  p->stepCurrent = fix16_from_float(cur);
}

void startStepResponse(void)
{
  s->cntrl_bit.stepResp = 1;
}

void resetStepRespVars(void)
{
  s->cntrl_bit.stepResp = 0;
  p->stepCurrent = 0;
  p->stepRespFlag = 0;
  p->stepRespCnt = 0;
}

void resetGP(void)
{
  s->cntrl_bit.resetGaitPhase = 1;
}


