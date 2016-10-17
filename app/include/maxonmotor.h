#ifndef _MAXONMOTOR_H_
#define _MAXONMOTOR_H_

#define ENABLE_PIN    9
#define DIR_PIN       10
#define MAX_DUTY      90
#define MIN_DUTY      10

#define FIX16_MAX_DUTY  0x5A0000
#define FIX16_MIN_DUTY  0xA0000

// Linear fit current -> PWMcmpValue
#define FIX16_K      0xFF999A     // 255.6 in fix16_t
#define FIX16_B      0x144E0000   // 5198 in fix16_t

#define FIX16_10      0xA0000     // 10 in fix16
#define FIX16_100     0x640000    // 100 in fix16
#define FIX16_50      0x320000    // 50 in fix16
#define FIX16_10000   0x27100000  // 10000 in fix16


// --- Digital output reg.
volatile register uint32_t __R30;

//---- Global ----
extern volatile uint32_t *debugBuffer;

// ---- Prototypes ----
void motorInit(void);
void motorCleanUp(void);
uint16_t motorCurrent2CmpValue(fix16_t u);
void motorSetCurrent(fix16_t u, volatile fix16_t *motorPwmCmpValue);
void motorEnable(void);
void motorDisable(void);

#endif
