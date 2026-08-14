/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * ADS131M08 device driver -- see ads131.h. Chip protocol (10-word frame,
 * RREG/WREG, reset + ID, sign extension, PGA gain) is shared verbatim with the
 * am64x branch; only the bus layer differs. Here that layer is the PRU's
 * spidriver (McSPI1 ch0, 24-bit, mode 1) plus a bit-banged R30 chip-select.
 */

#include "ads131.h"
#include "spidriver.h"

#define NWORD    (ADS131_NCH + 2u)   /* 10-word frame: resp + 8 data + CRC */

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

volatile register uint32_t __R30;

static ads131_cfg_t s_cfg;
static uint32_t     s_last_status;   /* frame word0 from the last ReadAllChannels */

static void spin(volatile uint32_t n) { while (n) { --n; } }

static int32_t sext24(uint32_t v)
{
  v &= 0x00FFFFFFu;
  return (v & 0x00800000u) ? (int32_t)(v | 0xFF000000u) : (int32_t)v;
}

/* One frame: CS low, exchange n words (CS held), CS high. */
static void frame(uint32_t* tx, uint32_t* rx, uint32_t n)
{
  __R30 &= ~(1u << s_cfg.cs_pin);
  spin(4u);
  spiMultiWordXfer(tx, rx, n);
  spin(4u);
  __R30 |= (1u << s_cfg.cs_pin);
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

uint16_t Ads131Init(const ads131_cfg_t* cfg)
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  s_cfg = *cfg;

  __R30 |= (1u << s_cfg.cs_pin);                        /* CS idle high        */

  tx[0] = PACK16(CMD_RESET);
  frame(tx, rx, NWORD);                                 /* RESET               */
  spin(400000u);                                        /* settle              */

  return Ads131ReadReg(REG_ID);                         /* 0x28xx expected     */
}

void Ads131ReadAllChannels(int32_t ch[ADS131_NCH])
{
  uint32_t tx[NWORD] = {0}, rx[NWORD] = {0};
  frame(tx, rx, NWORD);                                 /* NULL command        */
  s_last_status = rx[0];                                /* STATUS word         */
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
