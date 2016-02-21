#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "fix16.h"

#define FIX16_180           0xB40000
#define FIX16_360           0x1680000
#define FIX16_360_DIV_4096  0x1680


/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void encoderInit(void);
void encoderCleanUp(void);
void encoderSample(volatile fix16_t *pos);
fix16_t encoderCnts2Degs(uint16_t cnts);
void encoderSetZeroAngle(void);

#endif
