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
#define SIZE_OF_BUFFS   18

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
  volatile uint16_t anklePos;
  volatile uint16_t ankleVel;
  volatile uint16_t gaitPhase;
  volatile uint16_t motorDuty;
  volatile uint16_t adc[NUM_ADC];
  volatile int16_t imu[NUM_IMU];
} state_t;

/* Shared Memory */
typedef struct{

  /* Ping pong buffers */
  state_t state[NUM_OF_BUFFS][SIZE_OF_BUFFS];

  /* Flow control */
  union{
    volatile uint8_t flow_control;

    volatile struct{
      unsigned enable : 1;        // bit 0 (set by ARM, read by pru0)
      unsigned sensorDone : 1;    // bit 1 (set by pru0, read/reset by pru1)
      unsigned bufferFull: 1;     // bit 2 (set by pru1, read/reset by ARM)
      unsigned exit : 1;          // bit 3 (set by pru0, read by pru1)
      unsigned rsvd : 4;          // bits 4-7 reserved
   } flow_control_bit;
  };

} shared_mem_t;

/* pru0 (sensors) param memory */
typedef struct{
  volatile uint32_t frq_clock_ticks;
  volatile uint16_t gaitPhase_threshold_1;
  volatile uint16_t gaitPhase_threshold_2;
  volatile uint16_t gaitPhase_threshold_3;
  volatile uint16_t pad;
  volatile uint32_t debugBuffer[10];
} pru0_param_mem_t;

/* pru1 (control) param memory */
typedef struct{
  volatile uint16_t Kp;
  volatile uint16_t Kd;
  volatile uint16_t ankle_pos_0;
  volatile uint16_t ff_traj[100];
} pru1_param_mem_t;

#endif

