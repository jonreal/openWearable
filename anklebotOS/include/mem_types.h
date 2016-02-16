#ifndef _MEM_TYPES_
#define _MEM_TYPES_

#include "fix16.h"

/* Constants */
#define PRU_CLK         (200000000)
#define PWM_CLK         (100000000)
#define SPI_CLK         (48000000)
#define I2C1_CLK        (48000000)

#define HOST0_MASK    (0x40000000)
#define HOST1_MASK    (0x80000000)

#define PRU0_ARM_INT  (19 + 16)
#define PRU1_ARM_INT  (20 + 16)

#define NUM_OF_BUFFS    2
#define SIZE_OF_BUFFS   78

#define NUM_ADC     8
#define NUM_IMU     6

#define NUM_FF_LT         1000

/* Global Addresses to modules */
#define PRU_CTRL_BASE 0x00022000
#define ADC_BASE      0x44E0D000
#define SPI1_BASE     0x481A0000
#define PWM_SS2_BASE  0x48304000
#define EPWM2_BASE    0x48304200
#define I2C1_BASE     0x4802A000

/* Pru debug pins */
#define PRU0_DEBUG_PIN  5
#define PRU1_DEBUG_PIN  8

#define MAX_IIR_ORDER   3

#define PRU0 0
#define PRU1 1

/* Structures ---------------------------------------------------------------*/

/* Anklebot state */
typedef struct{

  volatile uint32_t timeStamp;
  volatile uint32_t sync;
  volatile uint32_t r_hsStamp;
  volatile uint32_t l_hsStamp;

  volatile uint16_t r_meanGaitPeriod;
  volatile uint16_t l_meanGaitPeriod;
  volatile uint16_t r_percentGait;
  volatile uint16_t l_percentGait;

  volatile int32_t motorDuty;
  volatile fix16_t anklePos;
  volatile fix16_t ankleVel;
  volatile fix16_t u_fb;
  volatile fix16_t u_ff;

  volatile int16_t adc[NUM_ADC];
  volatile int16_t imu[NUM_IMU];
  volatile int16_t heelForceVel[2];

} state_t;

/* Shared Memory -> mapped to SRAM */
typedef struct{

  /* Ping pong buffers */
  state_t state[NUM_OF_BUFFS][SIZE_OF_BUFFS];

  /* Flow control */
  union{
    volatile uint16_t cntrl;

    volatile struct{
      unsigned enable : 1;        // bit 0 (set by ARM and shadowed */
      unsigned pru0_done : 1;     // bit 1 (set by pru0, read/reset by pru1)
      unsigned pru1_done : 1;     // bit 2 (set by pru1, read/reset by pru0)
      unsigned buffer0_full : 1;  // bit 3 (set by pru0, read/reset by ARM)
      unsigned buffer1_full : 1;  // bit 4 (set by pru0, read/reset by ARM)
      unsigned shdw_enable : 1;   // bit 5 (shawdow reg. for enable)
      unsigned encoderTare : 1;   // bit 6 (set by arm, read/reset by pru1)
      unsigned doFeedForward : 1; // bit 7 (set by arm, read by pru1)
      unsigned resetGaitPhase: 1; // bit 8 (set by arm, read/reset by pru0)
      unsigned gaitPhaseReady: 1; // bit 9 (set by pru0, read by pru1)
      unsigned heelStrike : 1;    // bit 10 (set by pru0, reset by pru1)
      unsigned toeOff : 1;        // bit 11 (set by pru0, reset by pru1)
      unsigned testFF : 1;        // bit 12 (set by arm, reset by arm)
      unsigned stepResp : 1;      // bit 13 (set by arm, reset by pru1)
      unsigned rsvd : 2;          // bits 14-15 reserved
   } cntrl_bit;
  };

} shared_mem_t;

/* IIR Buffer */
typedef struct{
  volatile fix16_t x[MAX_IIR_ORDER+1];
  volatile fix16_t y[MAX_IIR_ORDER+1];
} iir_buff_t;

/* IIR Coefficients */
typedef struct{
  uint32_t N;
  fix16_t b[MAX_IIR_ORDER+1];
  fix16_t a[MAX_IIR_ORDER+1];
} iir_coeff_t;

/* Vel Buffer */
typedef struct{
  volatile fix16_t x[2];
  volatile fix16_t y[2];
} vel_buff_t;

/* Parameter Struct -> mapped to pr0 DRAM */
typedef struct{
  volatile uint32_t frq_hz;
  volatile uint32_t frq_clock_ticks;
  volatile uint32_t mass;
  volatile fix16_t Kp;
  volatile fix16_t Kd;
  volatile fix16_t anklePos0;

  volatile uint32_t stepRespCnt;
  volatile uint32_t stepRespFlag;
  volatile fix16_t stepCurrent;

  iir_coeff_t filt;
  iir_buff_t filtBuffer[6];
  vel_buff_t velBuffer[2];

  volatile uint32_t debugBuffer[10];
} param_mem_t;


/* Feedforward lookup table -> mapped to pru1 DRAM */
typedef struct{
  int16_t uff[NUM_FF_LT];
} lookUp_mem_t;

#endif

