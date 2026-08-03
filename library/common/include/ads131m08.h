/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * TI ADS131M08 -- 8-channel, 24-bit simultaneous-sampling delta-sigma ADC (SPI).
 * Core-agnostic device driver layered on the common McSPI driver (mcspi_j721e).
 * Used here from the R5F (McSPI7), but works from any core that can memmap McSPI.
 *
 * Frame model (default config): each SPI frame exchanges NWORD 24-bit words --
 *   word 0      : command in (MSB-aligned) / STATUS-response out
 *   words 1..8  : the 8 channel conversions (24-bit, two's complement)
 *   word 9      : CRC
 * A command's response arrives in the FOLLOWING frame. CS is held low across the
 * whole frame (mcspiFrameXfer / CHCONF.FORCE).
 *
 * NOTE: the ADС needs an external CLKIN (fMOD = fCLKIN/2); provide it separately.
 */

#ifndef OW_ADS131M08_H_
#define OW_ADS131M08_H_

#include <stdint.h>

#define ADS131_NCH     8u                 /* channels                          */
#define ADS131_NWORD   (ADS131_NCH + 2u)  /* 10-word frame: resp + 8 data + CRC */

#define ADS131_WLBITS  24u                /* SPI device word length            */
#define ADS131_SPIMODE 1u                 /* SPI mode 1 (CPOL=0, CPHA=1)        */

/* 16-bit command/register data sits in the 16 MSBs of a 24-bit word (8 LSBs 0).
 * Flip these two if bring-up shows the device wants LSB-aligned words. */
#define ADS131_PACK16(x)   (((uint32_t)(uint16_t)(x)) << 8)
#define ADS131_UNPACK16(w) ((uint16_t)(((w) >> 8) & 0xFFFFu))

/* Register addresses */
#define ADS131_REG_ID      0x00u
#define ADS131_REG_STATUS  0x01u
#define ADS131_REG_MODE    0x02u
#define ADS131_REG_CLOCK   0x03u
#define ADS131_REG_GAIN1   0x04u
#define ADS131_REG_GAIN2   0x05u
#define ADS131_REG_CFG     0x06u

/* System commands (16-bit) */
#define ADS131_CMD_NULL    0x0000u
#define ADS131_CMD_RESET   0x0011u
#define ADS131_CMD_STANDBY 0x0022u
#define ADS131_CMD_WAKEUP  0x0033u
#define ADS131_CMD_LOCK    0x0555u
#define ADS131_CMD_UNLOCK  0x0655u

typedef struct {
  int32_t  ch[ADS131_NCH];   /* sign-extended 24-bit conversions */
  uint32_t response;         /* frame word 0 (STATUS on a data read) */
  uint32_t crc;              /* frame word 9 */
} ads131_frame_t;

/* One-time setup: configure the McSPI channel (24-bit, mode 1) at spi_base and
 * the manual chip-select GPIO (cs_base + cs_bit = (1<<line)); RESET the ADC and
 * return its ID register. clkdiv selects SCLK (see mcspiInit). The config is
 * stored, so the read/write calls below take no bus args.
 *
 * CS is driven by GPIO, held low across the whole frame -- the OMAP McSPI FORCE
 * bit does NOT hold SPIEN across a multi-word frame on this IP (it reads word-0
 * ten times), so we bit-bang CS exactly like the AM335x reference. */
uint32_t ads131Init(uint32_t spi_base, uint16_t clkdiv, uint32_t cs_base, uint32_t cs_bit);

/* Read one conversion frame (response + 8 channels + CRC), CS held across it. */
void ads131ReadFrame(ads131_frame_t* f);

/* Single-register access (each costs one or two frames). */
uint16_t ads131ReadReg(uint8_t addr);
void     ads131WriteReg(uint8_t addr, uint16_t val);

#endif /* OW_ADS131M08_H_ */
