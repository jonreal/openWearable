/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * TI ADS131M08 driver -- see ads131m08.h. Layered on mcspi_j721e (memmap McSPI)
 * for the words and gpio_j721e (memmap GPIO) for a manual chip-select held low
 * across the whole frame. Pure memmap C; here linked into the R5F firmware.
 */

#include "ads131m08.h"
#include "mcspi_j721e.h"
#include "gpio_j721e.h"

/* RREG/WREG opcodes: 101/011 aaaa aan nnnn (addr, count-1). n=0 -> one register. */
#define ADS131_RREG(a, n) ((uint16_t)((0x5u << 13) | (((a) & 0x3Fu) << 7) | ((n) & 0x7Fu)))
#define ADS131_WREG(a, n) ((uint16_t)((0x3u << 13) | (((a) & 0x3Fu) << 7) | ((n) & 0x7Fu)))

/* Bus config, stored by ads131Init so the read/write calls stay argument-free. */
static uint32_t s_spi;       /* McSPI controller base */
static uint32_t s_cs_base;   /* CS GPIO instance base */
static uint32_t s_cs_bit;    /* CS GPIO bit = (1 << line) */

static void ads131_spin(volatile uint32_t n) { while (n) { --n; } }

/* Sign-extend a 24-bit two's-complement sample to int32. */
static int32_t ads131_sext24(uint32_t v)
{
  v &= 0x00FFFFFFu;
  return (v & 0x00800000u) ? (int32_t)(v | 0xFF000000u) : (int32_t)v;
}

/* One frame: CS low (GPIO), exchange n words, CS high. Held low the whole time. */
static void ads131_frame(const uint32_t* tx, uint32_t* rx, uint32_t n)
{
  gpioClr(s_cs_base, s_cs_bit);            /* CS low -> select, whole frame */
  ads131_spin(4u);                         /* CS setup */
  for (uint32_t i = 0u; i < n; ++i)
    rx[i] = mcspiXfer(s_spi, tx[i]);
  ads131_spin(4u);                         /* CS hold */
  gpioSet(s_cs_base, s_cs_bit);            /* CS high -> deselect */
}

uint32_t ads131Init(uint32_t spi_base, uint16_t clkdiv, uint32_t cs_base, uint32_t cs_bit)
{
  uint32_t tx[ADS131_NWORD] = {0};
  uint32_t rx[ADS131_NWORD] = {0};

  s_spi     = spi_base;
  s_cs_base = cs_base;
  s_cs_bit  = cs_bit;

  gpioInitOutput(cs_base, cs_bit);         /* CS idle high */
  mcspiInit(spi_base, ADS131_WLBITS, clkdiv, ADS131_SPIMODE);  /* 24-bit, mode 1 */

  /* RESET, then wait out the device settle before the first register access. */
  tx[0] = ADS131_PACK16(ADS131_CMD_RESET);
  ads131_frame(tx, rx, ADS131_NWORD);
  ads131_spin(400000u);                    /* >= reset settle (crude, generous) */

  return ads131ReadReg(ADS131_REG_ID);     /* caller checks 0x28xx family */
}

uint16_t ads131ReadReg(uint8_t addr)
{
  uint32_t tx[ADS131_NWORD] = {0};
  uint32_t rx[ADS131_NWORD] = {0};

  /* Frame 1: issue RREG (read one register). */
  tx[0] = ADS131_PACK16(ADS131_RREG(addr, 0u));
  ads131_frame(tx, rx, ADS131_NWORD);

  /* Frame 2: NULL -- the register value comes back in word 0 of this frame. */
  tx[0] = ADS131_CMD_NULL;
  ads131_frame(tx, rx, ADS131_NWORD);
  return ADS131_UNPACK16(rx[0]);
}

void ads131WriteReg(uint8_t addr, uint16_t val)
{
  uint32_t tx[ADS131_NWORD] = {0};
  uint32_t rx[ADS131_NWORD] = {0};

  tx[0] = ADS131_PACK16(ADS131_WREG(addr, 0u));
  tx[1] = ADS131_PACK16(val);
  ads131_frame(tx, rx, ADS131_NWORD);
}

void ads131ReadFrame(ads131_frame_t* f)
{
  uint32_t tx[ADS131_NWORD] = {0};         /* NULL command */
  uint32_t rx[ADS131_NWORD] = {0};
  uint32_t i;

  ads131_frame(tx, rx, ADS131_NWORD);

  f->response = rx[0];
  for (i = 0u; i < ADS131_NCH; ++i)
    f->ch[i] = ads131_sext24(rx[1u + i]);
  f->crc = rx[ADS131_NWORD - 1u];
}
