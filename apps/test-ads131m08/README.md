# test-ads131m08 — 8-channel, 24-bit ADC over SPI

The R5F reads all 8 channels of a TI **ADS131M08** over McSPI7 each tick; the A72
logs them as raw signed 24-bit codes. Bring-up/test app — no control loop.

## Wiring

| ADS131M08 | Signal    | Header | SoC ball | Pad          | Mux              |
|-----------|-----------|--------|----------|--------------|------------------|
| SCLK      | SPI clock | **P9.31** | U3    | `0x0011C234` | 6 = SPI7_CLK     |
| DOUT      | MISO      | **P9.30** | V6    | `0x0011C238` | 6 = SPI7_D0      |
| DIN       | MOSI      | **P9.29** | V5    | `0x0011C23C` | 6 = SPI7_D1      |
| CS        | chip-sel  | **P9.28** | U2    | `0x0011C230` | GPIO (bit-banged) |
| CLKIN     | master clk| **P9.14** | U27   | `0x0011C178` | 6 = EHRPWM2_A    |
| GND       | ground    | P9.1 / P9.2 | —   | —            | —                |
| AVDD/DVDD | 3.3 V     | P9.3 / P9.4 | —   | —            | —                |

SPI: mode 1, 24-bit words. CS is bit-banged over GPIO (the McSPI FORCE bit can't
hold it across a 10-word frame).

## CLKIN note

The ADS131M08 needs an external master clock. The J721E EHRPWM TBCLK is Linux-owned,
so the **A72 generates CLKIN** (7.8125 MHz) on EHRPWM2_A (P9.14) — the app does this
itself in `PruLoadParams`, so no setup is required. `enable-pwm.sh` does the same
bring-up standalone (e.g. to scope P9.14 without running the app).

## Build & run

```sh
cd library && make
cd apps/test-ads131m08 && make
apps/bin/test-ads131m08          # -v for per-record debug rows
```
