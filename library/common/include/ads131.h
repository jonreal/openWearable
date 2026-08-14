/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * TI ADS131M08 -- 8-channel, 24-bit simultaneous-sampling delta-sigma ADC.
 * AM335x port of the am64x driver (library/common/src/ads131.c on that branch):
 * identical chip protocol and API, different bus layer. Here the part hangs off
 * the PRU's McSPI1 channel 0 (library/pru/src/spidriver.c, already configured
 * 24-bit / mode 1 by spiInit) with chip-select bit-banged on a PRU R30 pin.
 *
 * Frame: each SPI transaction is 10 x 24-bit words -- word0 = command/STATUS,
 * words1..8 = channels, word9 = CRC. CS is held low across the whole frame.
 *
 * CLKIN: unlike am64x -- where Ads131Init brings up the EHRPWM itself -- the
 * AM335x master clock is generated in ARM space (EHRPWM2_A on P8.19 via sysfs;
 * see pwm_clkin_start() in apps/sample-ati/uiloop.c) because the PRU cannot gate
 * the Linux-owned TBCLK. Ads131Init assumes CLKIN is already running: without it
 * the modulator never converts and every frame returns the same post-reset
 * acknowledgment word (0xFF28) instead of data.
 */

#ifndef OW_ADS131_H_
#define OW_ADS131_H_

#include <stdint.h>

#define ADS131_NCH  8u

/* PGA gain codes (PGAGAINn[2:0]). */
typedef enum {
  ADS131_GAIN_1 = 0u, ADS131_GAIN_2, ADS131_GAIN_4,  ADS131_GAIN_8,
  ADS131_GAIN_16,     ADS131_GAIN_32, ADS131_GAIN_64, ADS131_GAIN_128
} ads131_gain_t;

/* Board wiring for one ADS131M08 instance. The SPI controller itself needs no
 * config here: spiInit() (called by the pru0 main loop) owns McSPI1 ch0. */
typedef struct {
  uint32_t cs_pin;     /* chip-select PRU R30 bit (6 = P9.27)              */
} ads131_cfg_t;

/* One-time setup: CS idle high + RESET + verify ID. Stores the config so the
 * calls below take no bus args. Returns the ID register (0x28xx family). */
uint16_t Ads131Init(const ads131_cfg_t* cfg);

/* Read one conversion frame -> 8 sign-extended 24-bit channels. */
void Ads131ReadAllChannels(int32_t ch[ADS131_NCH]);

/* Convenience: read a frame, return one channel. */
int32_t Ads131Read(uint8_t channel);

/* STATUS word (frame word0) captured by the last Ads131ReadAllChannels. */
uint32_t Ads131LastStatus(void);

/* Gain: per-channel (read-modify-write) and all-eight-at-once. */
void Ads131SetGain(uint8_t channel, ads131_gain_t gain);
void Ads131SetGainAll(const ads131_gain_t gain[ADS131_NCH]);

/* Raw register access (also used internally). */
uint16_t Ads131ReadReg(uint8_t addr);
void     Ads131WriteReg(uint8_t addr, uint16_t val);

#endif /* OW_ADS131_H_ */
