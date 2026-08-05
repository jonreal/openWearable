# test-ads131m08 — ADS131M08 8-channel, 24-bit ADC over SPI (R5F)

Reads all 8 channels of a **TI ADS131M08** (simultaneous-sampling, 24-bit,
delta-sigma ADC) on the **BeagleBone AI-64 (J721E/TDA4VM)**. The **R5F**
(`main_r5fss0`) drives the SPI bus and reads a conversion frame every (decimated)
tick; **PRU0** snapshots the result into the log ring; the **A72** runs the
UI/logger and generates the ADC master clock (CLKIN).

This is a bring-up/test app — no closed-loop control, just streaming the raw ADC.

---

## 1. Signal chain / who-owns-what

| Piece            | Owner        | Notes                                                        |
|------------------|--------------|-------------------------------------------------------------|
| SPI transfers    | **R5F**      | mmaps McSPI7 @ `0x02170000`, 24-bit words, mode 1            |
| Chip-select      | **R5F**      | bit-banged GPIO (`main_gpio1` @ `0x00601000`, line 11)      |
| CLKIN (~7.8 MHz) | **A72/Linux**| `pwm` framework on EHRPWM2_A — R5F *cannot* gate J721E TBCLK |
| Pin mux          | **DTB**      | `k3-j721e-boneai64-openWearable.dts` (see §3)                |
| UI + logging     | **A72**      | `uiloop.c` / `format.c`; host decoder `utils/logdecode/`     |

> **Why the A72 makes CLKIN:** the EHRPWM time-base clock (TBCLK) is a Linux-owned
> functional clock, gated by the `pwm-tiehrpwm` driver. It is only ungated when a
> **userspace** consumer enables the PWM. The R5F can write the EHRPWM registers but
> cannot turn TBCLK on, so `PruLoadParams` (in `uiloop.c`) enables it via sysfs
> before the R5F starts.

---

## 2. Wiring — BeagleBone AI-64 → ADS131M08

| ADS131M08 | Signal            | Header | SoC ball | Pad (CTRL_MMR) | Mux mode          |
|-----------|-------------------|--------|----------|----------------|-------------------|
| SCLK      | SPI clock         | **P9.31** | U3    | `0x0011C234`   | 6 = SPI7_CLK      |
| DOUT      | SPI MISO          | **P9.30** | V6    | `0x0011C238`   | 6 = SPI7_D0       |
| DIN       | SPI MOSI          | **P9.29** | V5    | `0x0011C23C`   | 6 = SPI7_D1       |
| CS        | chip-select       | **P9.28** | U2    | `0x0011C230`   | **7 = GPIO** (see note) |
| CLKIN     | master clock      | **P9.14** | U27   | `0x0011C178`   | **6 = EHRPWM2_A** |
| DGND/AGND | ground            | P9.1 / P9.2 | —    | —              | —                 |
| AVDD/DVDD | 3.3 V supply¹     | P9.3 / P9.4 | —    | —              | —                 |

¹ Verify against your breakout. ADS131M08 AVDD = 2.7–3.6 V, DVDD = 2.7–3.6 V. Many
  breakouts regulate/level-shift on-board — do **not** assume the header pin ties
  straight to the die.

**CS note:** the DTB muxes P9.28 as `SPI7_CS0` (mode 6), but the OMAP McSPI `FORCE`
bit can't hold `SPIEN` across a 10-word frame, so the `ads131` driver **re-muxes
P9.28 to GPIO (mode 7) at init** and bit-bangs CS itself. Nothing for you to do —
just don't expect a McSPI CS waveform on P9.28.

**SPI settings:** mode 1 (CPOL=0, CPHA=1), 24-bit words, 10 words/frame
(STATUS + 8 channels + CRC). CS must transition while SCLK is low.

---

## 3. Prerequisites (device tree) — one-time

The pinmux + peripherals come from `device-tree/k3-j721e-boneai64-openWearable.dts`
(symlinked into `~/BeagleBoard-DeviceTrees/src/arm64/ti/` on the board):

- `main_spi7_pins_default` — McSPI7 on P9.28–P9.31
- `epwm2_clkin_pins_default` — **P9.14 = EHRPWM2_A, mode 6** (mode 7 is gpio0_93;
  mode 6 is EHRPWM2_A — getting this wrong is the classic "no CLKIN" bug)
- `&main_ehrpwm2 { status = "okay"; }` and `&main_spi7 { status = "okay"; }`

**Editing the `.dts` is not enough** — you must recompile it into the `.dtb` and
reboot (a plain reboot reloads the *old* compiled DTB):

```sh
cd ~/BeagleBoard-DeviceTrees && make install_arm64   # compile + copy to /boot/firmware/ti
sudo reboot
```

Verify after reboot: `devmem2 0x0011C178 w` should read `0x00010006` (mode 6).

Run everything as **root** (needs `/dev/mem`, sysfs `pwm`, and runtime-PM writes).

---

## 4. Build

```sh
cd library            && make        # libow_pru.a (build first)
cd ../apps/test-ads131m08 && make    # A72 binary + PRU/R5F firmware, installs to apps/bin
```

## 5. Run

```sh
apps/bin/test-ads131m08          # add -v for per-record debug rows
#  s  start log (prompts for trial name)
#  d  stop log
#  e  exit
```

On start, `PruLoadParams`:
1. pins the McSPI7 functional clock on (`2170000.spi`),
2. enables CLKIN on EHRPWM2_A via the pwm framework (`3020000.pwm`, channel 0),
3. sets `fs = 1 kHz`, `r5f_decimate = 10` → ADC sampled at 100 Hz.

---

## 6. CLKIN frequency (ADS131M08 datasheet SBAS950B §6.3)

`fMOD = fCLKIN / 2` (high-resolution mode); `fDATA = fMOD / OSR`.

| Mode                      | Min | Nom   | Max     |
|---------------------------|-----|-------|---------|
| HR, gain 1 or 2           | 0.3 | 8.192 | **8.4** MHz |
| HR, gain > 2              | 0.3 | 8.192 | **8.2** MHz |
| Duty cycle                | 40  | 50    | 60 %    |

This app defaults to **7.8125 MHz** (period `128` ns, 50% duty in
`pwm_clkin_start`), which is in-spec for **every** PGA gain. To change it, edit the
`period`/`duty_cycle` in `uiloop.c`:
- `128` ns → 7.8125 MHz (all gains) ✅ default
- `122` ns → ~8.33 MHz (gain ≤ 2 only; exceeds the 8.2 MHz limit above gain 2)

(125 MHz TBCLK can't produce exactly 8.192 MHz, so we pick the nearest safe value.)

---

## 7. Output / channel mapping

Log fields (`format.c`):

| Field        | Type    | Meaning                                             |
|--------------|---------|-----------------------------------------------------|
| `frame`      | u32     | PRU tick counter                                    |
| `r5fvar`     | u32     | R5F loop counter (liveness)                         |
| `adc0..adc7` | i32     | **raw signed 24-bit codes**, channel *i* = AIN*i*   |
| `response`   | u32     | ADS131M08 ID register → **`0x2801`** if healthy     |
| `hlrev`      | u32     | McSPI7 `HL_REV` → `0x40301A0B` (bus reach check)    |

Codes are two's-complement, sign-extended to 32-bit. Convert to volts:

```
V = code × VREF / (gain × 2^23)        VREF = 1.2 V (internal reference)
```

At gain 1, 1 code ≈ 143 nV. The log stores **raw codes** (full 24-bit precision);
scale to mV in the host decoder rather than in firmware (fix16 mV would lose ~7
LSBs). Per-channel gain: `Ads131SetGain(ch, g)` / `Ads131SetGainAll(g[8])`.

---

## 8. Troubleshooting

| Symptom | Check |
|---------|-------|
| **No CLKIN on P9.14** | `devmem2 0x0011C178 w` must be `0x00010006` (mode 6). If `0x0001000B` (mode 11) or other → DTB not rebuilt: `make install_arm64` + reboot (§3). |
| **CLKIN dead even at mode 6** | The pwm must be enabled from **userspace**: `cat /sys/class/pwm/pwmchip1/pwm0/enable` should be `1`. The R5F cannot ungate TBCLK. |
| **All channels 0 / noisy garbage** | Almost always missing CLKIN — SPI/ID reads (`response=0x2801`) still work without it, but conversions don't. |
| **8 MHz "sine wave" on scope** | Normal probe-bandwidth rounding of an 8 MHz square; harmless as long as it crosses the CMOS thresholds. |
| **Shorted channel ≠ 0** | A true AINxP=AINxN short reads ≈0 *differential* regardless of ground; if not, check the breakout's input network and confirm the board-label ↔ `adc[]` mapping. |
| **`response` ≠ 0x2801** | SPI wiring / mode / CS — the ID read is independent of CLKIN. |

See also: `docs/COMM-BUS-LAYOUT.md` (McSPI partitioning) and the driver headers
`library/common/include/{ads131,mcspi_j721e,gpio_j721e,epwm_j721e}.h`.
