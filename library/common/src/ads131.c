/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * ADS131M08 device driver -- see ads131.h. Orchestrates the memmap primitives
 * (mcspi_j721e SPI, gpio_j721e CS, epwm_j721e CLKIN) and owns the chip protocol
 * (10-word frame, RREG/WREG). Pure memmap C; here linked into the R5F firmware.
 */

#include "ads131.h"
#include "mcspi_j721e.h"
#include "gpio_j721e.h"
#include "epwm_j721e.h"

#define HWREG(a) (*((volatile uint32_t *)(uintptr_t)(a)))

#define NWORD    (ADS131_NCH + 2u)   /* 10-word frame: resp + 8 data + CRC */
#define WLBITS   24u                 /* SPI device word length             */
#define SPIMODE  1u                  /* SPI mode 1 (CPOL=0, CPHA=1)         */

/* Registers */
#define REG_ID     0x00u
#define REG_GAIN1  0x04u             /* channels 0..3 */
#define REG_GAIN2  0x05u             /* channels 4..7 */

/* Commands + opcodes */
#define CMD_NULL   0x0000u
#define CMD_RESET  0x0011u
#define RREG(a, n) ((uint16_t)((0x5u << 13) | (((a) & 0x3Fu) << 7) | ((n) & 0x7Fu)))
#define WREG(a, n) ((uint16_t)((0x3u << 13) | (((a) & 0x3Fu) << 7) | ((n) & 0x7Fu)))

/* 16-bit command/register data occupies the 16 MSBs of a 24-bit word. */
#define PACK16(x)   (((uint32_t)(uint16_t)(x)) << 8)
#define UNPACK16(w) ((uint16_t)(((w) >> 8) & 0xFFFFu))

static ads131_cfg_t s_cfg;
static uint32_t      s_last_status;   /* frame word0 from the last ReadAllChannels */

static void spin(volatile uint32_t n) { while (n) { --n; } }

static int32_t sext24(uint32_t v)
{
  v &= 0x00FFFFFFu;
  return (v & 0x00800000u) ? (int32_t)(v | 0xFF000000u) : (int32_t)v;
}

/* One frame: GPIO CS low, exchange n words (CS held), CS high. */
static void frame(const uint32_t* tx, uint32_t* rx, uint32_t n)
{
  gpioClr(s_cfg.cs_base, s_cfg.cs_bit);
  spin(4u);
  for (uint32_t i = 0u; i < n; ++i)
    rx[i] = mcspiXfer(s_cfg.spi_base, tx[i]);
  spin(4u);
  gpioSet(s_cfg.cs_base, s_cfg.cs_bit);
}

uint16_t Ads131ReadReg(uint8_t addr)
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  tx[0] = PACK16(RREG(addr, 0u));
  frame(tx, rx, NWORD);              /* frame 1: issue RREG            */
  tx[0] = CMD_NULL;
  frame(tx, rx, NWORD);              /* frame 2: value in word0        */
  return UNPACK16(rx[0]);
}

void Ads131WriteReg(uint8_t addr, uint16_t val)
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  tx[0] = PACK16(WREG(addr, 0u));
  tx[1] = PACK16(val);
  frame(tx, rx, NWORD);
}

/* P9.28 -> GPIO output mux (mode 7 + input buffer), matching adc-setup.sh. */
static void pad_gpio_output(uint32_t pad)
{
  if (pad)
    HWREG(pad) = 0x00050007u;
}

uint16_t Ads131Init(const ads131_cfg_t* cfg)
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  s_cfg = *cfg;

  if (cfg->pwm_base)                                    /* CLKIN (0 = external) */
    epwmInitClock(cfg->pwm_base, cfg->clkin_hz);
  pad_gpio_output(cfg->cs_pad);                         /* CS pad -> GPIO       */
  gpioInitOutput(cfg->cs_base, cfg->cs_bit);            /* CS idle high         */
  mcspiInit(cfg->spi_base, WLBITS, cfg->sclk_div, SPIMODE);  /* 24-bit, mode 1  */

  tx[0] = PACK16(CMD_RESET);
  frame(tx, rx, NWORD);                                 /* RESET               */
  spin(400000u);                                        /* settle              */

  return Ads131ReadReg(REG_ID);                         /* 0x28xx expected      */
}

void Ads131ReadAllChannels(int32_t ch[ADS131_NCH])
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  frame(tx, rx, NWORD);                                 /* NULL command         */
  s_last_status = rx[0];                                /* STATUS word          */
  for (uint32_t i = 0u; i < ADS131_NCH; ++i)
    ch[i] = sext24(rx[1u + i]);
}

uint32_t Ads131LastStatus(void)
{
  return s_last_status;
}

int32_t Ads131Read(uint8_t channel)
{
  int32_t ch[ADS131_NCH];
  Ads131ReadAllChannels(ch);
  return ch[channel & 0x7u];
}

void Ads131SetGain(uint8_t channel, ads131_gain_t gain)
{
  uint8_t  reg = (channel < 4u) ? REG_GAIN1 : REG_GAIN2;
  uint8_t  pos = (uint8_t)((channel & 0x3u) * 4u);      /* 3-bit field, 4-bit stride */
  uint16_t v   = Ads131ReadReg(reg);
  v = (uint16_t)((v & ~(0x7u << pos)) | (((uint16_t)gain & 0x7u) << pos));
  Ads131WriteReg(reg, v);
}

void Ads131SetGainAll(const ads131_gain_t g[ADS131_NCH])
{
  uint16_t g1 = (uint16_t)((g[0] & 7u) | ((g[1] & 7u) << 4) | ((g[2] & 7u) << 8) | ((g[3] & 7u) << 12));
  uint16_t g2 = (uint16_t)((g[4] & 7u) | ((g[5] & 7u) << 4) | ((g[6] & 7u) << 8) | ((g[7] & 7u) << 12));
  Ads131WriteReg(REG_GAIN1, g1);
  Ads131WriteReg(REG_GAIN2, g2);
}
