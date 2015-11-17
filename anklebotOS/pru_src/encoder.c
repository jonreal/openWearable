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

void encoderSample(volatile int16_t *pos)
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

  *pos =  encoderCnts2Degs((uint16_t) ((MSB << 8) | (LSB & 0xFF)));
}

int16_t encoderCnts2Degs(uint16_t cnts)
{
  int32_t angleDeg = ((int32_t)cnts)*36000/4096;

   if(angleDeg > 18000){
     angleDeg = angleDeg - 36000;
  }
  return (int16_t)(angleDeg + ANGLE_OFFSET);
}

void encoderSetZeroAngle(void)
{
  uint8_t rx = 0;

  /* Issue read command (0x70) */
  rx = spiXfer(0x70);

  /* Wait till recieve ack. (0x80) */
  while(rx != 0x80){
    rx = spiXfer(0x00);
  }
}
