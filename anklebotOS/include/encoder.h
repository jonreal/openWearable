#ifndef _ENCODER_H_
#define _ENCODER_H_

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void encoderInit(void);
void encoderCleanUp(void);
void encoderSample(volatile uint16_t *pos);

#endif
