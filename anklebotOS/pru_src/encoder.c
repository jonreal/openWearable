#include <stdint.h>

#include "spidriver.h"
#include "encoder.h"

void encoderInit(void)
{
 spiInit();
}

void encoderCleanUp(void)
{
  spiCleanUp();
}

void encoderSample(volatile uint16_t *pos)
{
  uint8_t rx = 0;
  uint8_t MSB = 0;
  uint8_t LSB = 0;

  /* Issue read command (0x10) */
  rx = spiXfer(0x10);

  /* Wait till recieve ack. (0x10) */
  while(rx != 0x10){
    rx = spiXfer(0x00);
  }

  /* RX data */
  MSB = spiXfer(0x00);
  LSB = spiXfer(0x00);

  *pos =  (uint16_t) ((MSB << 8) | (LSB & 0xFF));
}
