#include <stdint.h>

#include "spidriver.h"
#include "lsm6ds3imu.h"

// -- Protocol ----------------------------------------------------------------
// bit 0:       0 for write, 1 for read
// bit 1-7:     register address
// bit 8-15:    data


void lsm_imuInit(void)
{
  uint16_t rx = 0;
  uint16_t cmd = 0x8F00;
//  uint16_t cmd = 0x19 << 1 | 0x1;

  rx = spiXfer(SPI_IMU_CHAN, 0x0);

  debugBuffer[0] = 0x0;
  debugBuffer[1] = rx;

  __delay_cycles(10000);

  rx = spiXfer(SPI_IMU_CHAN, cmd);

  debugBuffer[2] = cmd;
  debugBuffer[3] = rx;

  __delay_cycles(10000);

  rx = spiXfer(SPI_IMU_CHAN, 0x0);

  debugBuffer[4] = 0x0;
  debugBuffer[5] = rx;
}

void lsm_x(void)
{
  uint16_t rx = 0;
  uint16_t cmd = 0x8F00;
//  uint16_t cmd = 0x19 << 1 | 0x1;

  rx = spiXfer(SPI_IMU_CHAN, cmd);

}
