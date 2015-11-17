#ifndef _MEM_TYPES_
#define _MEM_TYPES_

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
#define SIZE_OF_BUFFS   128

#define NUM_ADC     8
#define NUM_IMU     6

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

/* Enums --------------------------------------------------------------------*/
enum{
  LOAD = 0,
  UNLOAD,
  SWING
};

/* Structures ---------------------------------------------------------------*/

/* Anklebot state */
typedef struct{
  volatile uint32_t timeStamp;
  volatile int16_t anklePos;
  volatile uint16_t ankleVel;
  volatile uint16_t gaitPhase;
  volatile int16_t motorDuty;
  volatile uint16_t adc[NUM_ADC];
  volatile int16_t imu[NUM_IMU];
} state_t;

/* Shared Memory -> mapped to SRAM */
typedef struct{

  /* Ping pong buffers */
  state_t state[NUM_OF_BUFFS][SIZE_OF_BUFFS];

  /* Flow control */
  union{
    volatile uint8_t cntrl;

    volatile struct{
      unsigned enable : 1;        // bit 0 (set by ARM and shadowed */
      unsigned pru0_done : 1;     // bit 1 (set by pru0, read/reset by pru1)
      unsigned pru1_done : 1;     // bit 2 (set by pru1, read/reset by pru0)
      unsigned buffer0_full : 1;  // bit 3 (set by pru0, read/reset by ARM)
      unsigned buffer1_full : 1;  // bit 4 (set by pru0, read/reset by ARM)
      unsigned shdw_enable : 1;   // bit 5 (shawdow reg. for enable)
      unsigned encoderTare : 1;   // bit 6 (set by arm, read/reset by pru1)
      unsigned rsvd : 1;          // bits 5-7 reserved
   } cntrl_bit;
  };

} shared_mem_t;


/* Parameter Struct -> mapped to pr0 DRAM */
typedef struct{
  volatile uint32_t frq_clock_ticks;
  volatile uint16_t gaitPhase_threshold_1;
  volatile uint16_t gaitPhase_threshold_2;
  volatile uint16_t gaitPhase_threshold_3;
  volatile int16_t Kp;
  volatile int16_t Kd;
  volatile int16_t anklePos0;
  volatile uint32_t debugBuffer[10];
} param_mem_t;

/* Feedforward lookup table -> mapped to pru1 DRAM */
typedef struct{
  volatile uint16_t ff_traj[100];
} ff_mem_t;

#endif

