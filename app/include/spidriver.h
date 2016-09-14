#ifndef _SPIDRIVER_H_
#define _SPIDRIVER_H_

// Global ---------------------------------------------------------------------
extern volatile uint32_t *debugBuffer;

// Prototypes -----------------------------------------------------------------
void spiInit(void);
void spiCleanUp(void);
uint16_t spiXfer(uint8_t channel, uint16_t tx);

#endif
