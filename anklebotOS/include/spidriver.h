#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

/* ---- Global ---- */
extern volatile uint32_t *debugBuffer;

/* ---- Prototypes ---- */
void spiInit(void);
void spiCleanUp(void);
uint8_t spiXfer(uint8_t tx);

#endif
