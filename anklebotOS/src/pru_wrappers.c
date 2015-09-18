#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "mem_types.h"
#include "pru_wrappers.h"

#define AM33XX

/* Pointer to state data */
shared_mem_t *p;

/* Pointer to sensor pru params */
pru0_param_mem_t *pru0_param;

/* Pointer to controller pru params */
pru1_param_mem_t *pru1_param;

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

  sleep(2);

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

  /* Memory Map for pru0 DRAM */
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  pru0_param = (pru0_param_mem_t *) ptr;

  /* Memory Map for pru1 DRAM */
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  pru1_param = (pru1_param_mem_t *) ptr;

  /* Memory Map for shared memory */
  ptr = NULL;
  if( (rtn = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &ptr)) != 0){
    printf("prussdrv_map_prumem() failed with %i\n", rtn);
    return -1;
  }
  p = (shared_mem_t *) ptr;

  printf("Memory Allocation:\n");
  printf("\tSize of pru0 param memory: %i bytes.\n", sizeof(*pru0_param)*CHAR_BIT);
  printf("\tSize of pru1 param memory: %i bytes.\n", sizeof(*pru1_param)*CHAR_BIT);
  printf("\tSize of shared memory: %i bytes.\n", sizeof(*p)*CHAR_BIT);

  /* Zero Memory */
  for(int i=0; i<NUM_OF_BUFFS; i++){
    for(int j=0; j<SIZE_OF_BUFFS; j++){
      p->state[i][j].timeStamp = 0;
    //  p->state[i][j].ankle_pos = 0;
    //  p->state[i][j].ankle_vel = 0;
      p->state[i][j].adc_value[0] = 0;
      p->state[i][j].adc_value[1] = 0;
      p->state[i][j].adc_value[2] = 0;
      p->state[i][j].adc_value[3] = 0;
      p->state[i][j].adc_value[4] = 0;
      p->state[i][j].adc_value[5] = 0;
      p->state[i][j].adc_value[6] = 0;
     // p->state[i][j].imu_value[0] = 0;
     // p->state[i][j].imu_value[1] = 0;
     // p->state[i][j].imu_value[2] = 0;
     // p->state[i][j].imu_value[3] = 0;
     // p->state[i][j].imu_value[4] = 0;
     // p->state[i][j].imu_value[5] = 0;
     // p->state[i][j].imu_value[6] = 0;
     // p->state[i][j].imu_value[7] = 0;
     // p->state[i][j].imu_value[8] = 0;
     // p->state[i][j].motor_current_cmd = 0;
     // p->state[i][j].gaitPhase = 0;
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

/* ----------------------------------------------------------------------------
 * Function: void writePruSensorParams(float frq_hz, uint32_t gp_thr_1,
 *                        uint32_t gp_thr_2, uint32_t gp_thr_3)
 *
 * This function writes params to pru0 (sensor) DRAM
 *
 * Inputs:  frq_hz    -     loop freq in hz (float)
 *          gp_thr_n  -     gaitphase threshold n
 * ------------------------------------------------------------------------- */
void writePruSensorParams(float frq_hz, uint32_t gp_thr_1,
                          uint32_t gp_thr_2, uint32_t gp_thr_3)
{
  pru0_param->frq_clock_ticks = hzToPruTicks(frq_hz);
  pru0_param->gaitPhase_threshold_1 = gp_thr_1;
  pru0_param->gaitPhase_threshold_2 = gp_thr_2;
  pru0_param->gaitPhase_threshold_3 = gp_thr_3;
}

/* ----------------------------------------------------------------------------
 * Function: void writePruConrtolParams(uint32_t Kp, uint32_t Kd, uint32_t pos0,
 *                        uint32_t ff_traj[100])
 *
 * This function writes params to pru1 (control) DRAM
 *
 * Inputs:  Kp        -     proportional impedance gain
 * ------------------------------------------------------------------------- */
void writePruConrtolParams(uint32_t Kp, uint32_t Kd, uint32_t pos0,
                          uint32_t ff_traj[100])
{
  pru1_param->Kp = Kp;
  pru1_param->Kd = Kd;
  pru1_param->ankle_pos_0 = pos0;

  for(int i=0; i<100; i++){
    pru1_param->ff_traj[i] = ff_traj[i];
  }
}

/* ----------------------------------------------------------------------------
 * Function: void writePruConrtolParams(uint32_t Kp, uint32_t Kd, uint32_t pos0,
 *                        uint32_t ff_traj[100])
 *
 * This function writes params to pru1 (control) DRAM
 *
 * Inputs:  Kp        -     proportional impedance gain
 * ------------------------------------------------------------------------- */
void writeState(uint8_t bi)
{
  for(int i=0; i<SIZE_OF_BUFFS; i++){
    printf("0x%X\t", p->state[bi][i].timeStamp);

    printf("0x%X\t", p->state[bi][i].ankle_pos);
    printf("0x%X\t", p->state[bi][i].ankle_vel);
    printf("0x%X\t", p->state[bi][i].gaitPhase);
    printf("%i\t", p->state[bi][i].motor_current_cmd);

    printf("%i\t", p->state[bi][i].adc_value[0]);
    printf("%i\t", p->state[bi][i].adc_value[1]);
    printf("%i\t", p->state[bi][i].adc_value[2]);
    printf("%i\t", p->state[bi][i].adc_value[3]);
    printf("%i\t", p->state[bi][i].adc_value[4]);
    printf("%i\t", p->state[bi][i].adc_value[5]);
    printf("%i\t", p->state[bi][i].adc_value[6]);
    printf("%i\t", p->state[bi][i].adc_value[7]);

    printf("%i\t", p->state[bi][i].imu_value[0]);
    printf("%i\t", p->state[bi][i].imu_value[1]);
    printf("%i\t", p->state[bi][i].imu_value[2]);
    printf("%i\t", p->state[bi][i].imu_value[3]);
    printf("%i\t", p->state[bi][i].imu_value[4]);
    printf("%i\t", p->state[bi][i].imu_value[5]);
    printf("%i\t", p->state[bi][i].imu_value[6]);
    printf("%i\t", p->state[bi][i].imu_value[7]);
    printf("%i\t", p->state[bi][i].imu_value[8]);
    printf("%i\t", p->state[bi][i].imu_value[9]);

    printf("\n");

/*
    fprintf(fid,"%i ", p->state[bi][i].timeStamp);
    fprintf(fid,"%i ", p->state[bi][i].ankle_pos);
    fprintf(fid,"%i ", p->state[bi][i].ankle_vel);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[0]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[1]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[2]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[3]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[4]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[5]);
    fprintf(fid,"%i ", p->state[bi][i].adc_value[6]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[0]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[1]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[2]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[3]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[4]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[5]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[6]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[7]);
    fprintf(fid,"%i ", p->state[bi][i].imu_value[8]);
    fprintf(fid,"%i ", p->state[bi][i].motor_current_cmd);
    fprintf(fid,"%i ", p->state[bi][i].gaitPhase);
    fprintf(fid,"\n");
*/
    /* Zero State */
    p->state[bi][i].timeStamp = 0;

    p->state[bi][i].ankle_pos = 0;
    p->state[bi][i].ankle_vel = 0;
    p->state[bi][i].gaitPhase = 0;
    p->state[bi][i].motor_current_cmd = 0;

    p->state[bi][i].adc_value[0] = 0;
    p->state[bi][i].adc_value[1] = 0;
    p->state[bi][i].adc_value[2] = 0;
    p->state[bi][i].adc_value[3] = 0;
    p->state[bi][i].adc_value[4] = 0;
    p->state[bi][i].adc_value[5] = 0;
    p->state[bi][i].adc_value[6] = 0;
    p->state[bi][i].adc_value[7] = 0;

    p->state[bi][i].imu_value[0] = 0;
    p->state[bi][i].imu_value[1] = 0;
    p->state[bi][i].imu_value[2] = 0;
    p->state[bi][i].imu_value[3] = 0;
    p->state[bi][i].imu_value[4] = 0;
    p->state[bi][i].imu_value[5] = 0;
    p->state[bi][i].imu_value[6] = 0;
    p->state[bi][i].imu_value[7] = 0;
    p->state[bi][i].imu_value[8] = 0;
    p->state[bi][i].imu_value[9] = 0;

  }
  printf("\n");
}

void clearFlowBitFeild(void)
{
  p->flow_control = 0x00;
}

void enable(void)
{
  p->flow_control_bit.enable = 1;
}

void disable(void)
{
  p->flow_control_bit.enable = 0;
}

int isBufferFull(void)
{
  return (int) p->flow_control_bit.bufferFull;
}

void resetBufferFullFlag(void)
{
  p->flow_control_bit.bufferFull = 0;
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
