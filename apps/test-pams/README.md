# test-pams — single PAM: I2C pressure sampling + valve control

PRU1 samples a pressure sensor over **I2C2** (through a PCA9548 mux) and drives the
valves; set the target pressure `Pd` from the UI.

## I2C2 wiring (`0x02020000`)

| Signal | Header | SoC ball | Pad (CTRL_MMR) | Mux mode     |
|--------|--------|----------|----------------|--------------|
| SCL    | **P9.19** | W5    | `0x0011C208`   | 4 = I2C2_SCL |
| SDA    | **P9.20** | W6    | `0x0011C20C`   | 4 = I2C2_SDA |
| GND    | P9.1 / P9.2 | —   | —              | —            |
| 3.3 V  | P9.3 / P9.4 | —   | —              | —            |

I2C addresses: PCA9548 mux `0x70`, pressure sensor (HSC/SSC) `0x28`.

## Build & run

```sh
cd library && make
cd apps/test-pams && make
apps/bin/test-pams
```
