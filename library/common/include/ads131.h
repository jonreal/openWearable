/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * TI ADS131M08 -- 8-channel, 24-bit simultaneous-sampling delta-sigma ADC.
 * Self-contained device driver: Ads131Init sets up the CLKIN (epwm_j721e), the
 * chip-select GPIO incl. its pad mux (gpio_j721e + CTRL_MMR), and the SPI channel
 * (mcspi_j721e), then resets the part and verifies its ID. Named per family so a
 * future ADC gets its own ads127/Ads127* without colliding.
 *
 * Frame: each SPI transaction is 10 x 24-bit words -- word0 = command/STATUS,
 * words1..8 = channels, word9 = CRC. CS (GPIO) is held low across the whole
 * frame because the OMAP McSPI FORCE bit can't hold SPIEN on this IP.
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

/* Board wiring for one ADS131M08 instance. */
typedef struct {
  uint32_t spi_base;   /* McSPI controller base (e.g. MCSPI7_BASE)          */
  uint16_t sclk_div;   /* SPI clock divider (mcspiInit clkdiv)              */
  uint32_t cs_base;    /* CS GPIO instance base (e.g. GPIO1_BASE)           */
  uint32_t cs_bit;     /* CS GPIO bit = (1u << line)                        */
  uint32_t cs_pad;     /* CS pad CTRL_MMR addr to mux to GPIO; 0 = skip     */
  uint32_t pwm_base;   /* EHRPWM base for CLKIN (e.g. EPWM2_BASE)           */
  uint32_t clkin_hz;   /* CLKIN frequency (e.g. 8192000)                    */
} ads131_cfg_t;

/* One-time setup: CLKIN + CS(+pad) + SPI + RESET + verify ID. Stores the config
 * so the calls below take no bus args. Returns the ID register (0x28xx family). */
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
