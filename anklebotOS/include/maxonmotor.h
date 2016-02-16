#ifndef _MAXONMOTOR_H_
#define _MAXONMOTOR_H_

#define ENABLE_PIN    9
#define DIR_PIN       10
#define MAX_DUTY      (90.0)
#define MIN_DUTY      (10.0)

/* --- Digital output reg. */
volatile register uint32_t __R30;

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void motorInit(void);
void motorCleanUp(void);
void motorSetDuty(int16_t duty, volatile int32_t *motorDuty);
void motorEnable(void);
void motorDisable(void);
void motorSetDir(uint8_t dorsiflex);
uint16_t duty2cmpval(uint16_t duty);

#endif
