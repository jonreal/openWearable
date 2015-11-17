#ifndef _ENCODER_H_
#define _ENCODER_H_

#define ANGLE_OFFSET 0

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void encoderInit(void);
void encoderCleanUp(void);
void encoderSample(volatile int16_t *pos);
int16_t encoderCnts2Degs(uint16_t cnts);
void encoderSetZeroAngle(void);

#endif
