# Communication-bus layout & device-tree partition (J721E / BeagleBone AI-64)

How the SPI / I²C / PRU-GPO resources of the **TDA4VM (J721E)** map onto the BeagleBone AI-64
expansion headers, and the **device-tree partition** for the current milestone (ICSSG0):

- **`pru0_0` and `pru0_1`** each get a **dedicated hardware SPI**, driven by memory-mapping the
  McSPI controller (not bit-banged), plus **as much GPIO as the slice has on the headers**;
- the **R5F** and the **A72 (Linux)** each get a **dedicated I²C** (slower, fine for them);
- every remaining PRG0 pad is a **PRU digital output (GPO)**.

All numbers below were taken from the live board (`/proc/device-tree`, sysfs), the BeagleBone
AI-64 P8/P9 pin map, and the J721E control-module register table — they are reproducible on any
BBAI-64.

---

## 1. The board's pin field is just P8 + P9

The "two large connectors" on the board are the **P8 (2×23)** and **P9 (2×23)** cape headers —
together they are the *entire* general-purpose pin field on the BeagleBone AI-64. Every SPI, I²C,
GPIO and PRU signal in this document comes out on one of those two headers.

The board's other connectors are **fixed-function** and add no general-purpose SPI/I²C/GPIO:

| Connector            | Function (dedicated)          | Usable as general bus/GPIO? |
|----------------------|-------------------------------|-----------------------------|
| 2× CSI (FFC ribbon)  | MIPI CSI-2 camera lanes       | No (MIPI D-PHY only)        |
| DSI (FFC)            | MIPI DSI display              | No                          |
| M.2 E-key            | PCIe / SDIO / USB (Wi-Fi)     | No                          |
| mikroBUS socket      | shares P8/P9-routed SPI/I²C/UART/PWM | counts against the P8/P9 budget, not extra |
| USB-C / USB3 / USB2  | USB                           | No                          |

So **the P8/P9 budget below is the whole budget.**

---

## 2. SoC bus inventory — silicon vs. what reaches the headers

### SPI — 11 controllers, only 3 reach a header as a *complete* bus
`/proc/device-tree` shows 8 MAIN MCSPI (`spi@2100000`…`2170000`, base `0x2100000 + n·0x10000`)
plus 3 MCU SPI. All 8 MAIN MCSPI are `status = "disabled"` in Linux → none bound, all free to assign.

| Controller | Base addr   | Complete bus on P8/P9? | Pads land on…                    |
|------------|-------------|------------------------|----------------------------------|
| **SPI7**   | `0x2170000` | ✅ CLK+D0+D1+CS0 (full-duplex, 1 CS) | **non-PRU** (timer/ecap pads) |
| **SPI6**   | `0x2160000` | ✅ CLK+D0+D1+CS0..3 (full-duplex, 4 CS) | **ICSSG1 / PRG1 PRU pads** |
| **SPI3**   | `0x2130000` | ◑ CLK+D0+CS0+CS3 (3-wire, 2 CS — no D1 on header) | **ICSSG0 / PRG0 PRU pads** |
| SPI0/1/2   | —           | ✗ fragmentary (CLK or D0 missing; pads stolen by I²C4/6) | mixed |
| SPI4/5     | —           | ✗ not bonded to headers at all | — |
| MCU_SPI0/1/2 | MCU dom.  | MCU_SPI1 on P9 but in the MCU power domain (cross-domain, avoid) | WKUP pads |

> **Key fact:** **SPI7 is the only complete SPI bus on a header that does *not* consume PRU pads.**
> SPI6 and SPI3 carry their pads on one ICSSG instance's PRU pool — fine when that ICSSG is unused
> (we use SPI6's controller while ICSSG1 is idle), but never put SPI3 on PRG0 while ICSSG0 is in use.

### I²C — 10 controllers, but only 2 are cleanly on the headers
`i2c@2000000`…`2060000` = MAIN I2C0–6 (7); `mcu_i2c0/1` + `wkup_i2c0` (3) are MCU/WKUP-domain
(onboard PMIC/EEPROM/DP). All 7 MAIN I²C are `okay` in Linux, but enabled ≠ on the headers.

| Controller | Base addr   | Both lines on a numbered header pin? |
|------------|-------------|--------------------------------------|
| **I2C2**   | `0x2020000` | ✅ P9.19 / P9.20 (MCAN0 pads) |
| **I2C4**   | `0x2040000` | ✅ P9.24 / P9.26 (SPI1 pads) |
| I2C3       | `0x2030000` | ◑ SCL P8.13 clean, **SDA on a non-header ball (U28)** |
| I2C6       | `0x2060000` | ◑ both lines on non-header balls (AA3 / Y2) |

> Only **I2C2 and I2C4** have both lines on clean header pins. This is *the* I²C constraint, and the
> reason the PRUs go to SPI: it drops I²C demand to two consumers (R5F + ARM) = exactly these two.

### PRU digital outputs — 80 in silicon, 60 on the headers
**2× ICSSG** (`icssg@b000000` = PRG0, `icssg@b100000` = PRG1). Each ICSSG = 2 slices; each slice =
**20 GPO** (`GPO0..19`) in silicon. The same pads do GPI (input) in mux mode 1. Counting only pads
on a numbered header pin: `pru0_0` = **9**, `pru0_1` = **14** (the others land on non-header balls).
Within a slice the PRU / RTU / TX_PRU cores **share** the pads (driven via `R30`, read via `R31`).

---

## 3. Hard constraints that shape the partition

1. **One master per controller instance.** A given MCSPI/I²C register block is driven by exactly one
   core at a time. "Independent SPI/I²C per core" means **separate instances** — fine, there are plenty.
2. **Only 2 I²C buses (I2C2, I2C4) are cleanly on the headers** — so reserve I²C for the two least
   numerous consumers (R5F + ARM) and put the PRUs on SPI.
3. **SPI7 is the only non-PRU complete SPI bus.** A second header SPI (SPI3/SPI6) carries its pads on
   an ICSSG PRU pool — only spend those while that ICSSG is idle.
4. **PRU/R5F ownership of a hardware controller is more than DT.** Linux must *not* bind a driver to
   it (`status="disabled"`), the **A72 enables its functional clock** (TISCI) and the **firewall must
   admit the owning master-ID**. The DT only sets pads + keeps Linux off. The `mcspiHlRev()` readback
   is the smoke test for reachability.

---

## 4. The target partition (current target: ICSSG0 only)

The active milestone uses **only the two PRUs of ICSSG0** — `pru0_0` and `pru0_1`. Each PRU gets a
**dedicated hardware SPI**, driven by **memory-mapping the McSPI controller** (not bit-banged — §6).
I²C is reserved for the **R5F and the A72**. Every remaining PRG0 pad stays a **PRU GPO**.

| Core | Bus (hardware, mem-mapped) | Base | Header pins | PRG0 GPIO kept |
|------|----------------------------|------|-------------|----------------|
| **pru0_0** | **McSPI7** (full-duplex, 1 CS) | `0x2170000` | P9.28/29/30/31 | **9** (R30 bits 2,3,4,5,8,16-19) |
| **pru0_1** | **McSPI6** (full-duplex, ≤4 CS) | `0x2160000` | P9.17/18/21/22 (+CS1-3 spare) | **14** (R30 bits 2-13,16,17) |
| **R5F** | **I2C2** | `0x2020000` | P9.19 / P9.20 ✅ | — |
| **A72/ARM** | **I2C4** (`/dev/i2c-3`) | `0x2040000` | P9.24 / P9.26 ✅ | — |

**Why this works — zero PRG0 cost.** Every non-PRU bus lands on a *non-PRG0* pad: McSPI7 on
timer/ecap pads, McSPI6 on ICSSG1/PRG1 pads (the *controller* drives them — ICSSG1 itself is unused
here), I2C2 on MCAN0 pads, I2C4 on SPI1 pads. So **both PRUs keep 100% of their header GPIO** (9 +
14); nothing is subtracted "for the other processors." This is why we use SPI6/SPI7 and **not** SPI3
(SPI3 would consume pru0_1's GPO7/9/16/17).

> ⚠️ **Board-power pads — do NOT remux.** Three PRG0 pads are already board functions and stay
> mode-7 GPIO: `PRG0_PRU0_GPO6` (`0xc8`, DP 3v3 enable), `PRG0_PRU1_GPO19` (`0x14c`, **SD card
> power**), `PRG0_PRU1_GPO14/15` (`0x138`/`0x13c`). None are header pins (so they're outside the
> 9/14 counts) — just never fold them into the PRU GPO groups.

### Exact header pins / IOPAD offsets (`&main_pmx0`)

| Bus | CLK / D0(MISO) / D1(MOSI) / CS0 (header) | IOPAD | Mux |
|-----|------------------------------------------|-------|-----|
| McSPI7 (pru0_0) | P9.31 / P9.30 / P9.29 / P9.28 | 0x234 / 0x238 / 0x23c / 0x230 | 6 |
| McSPI6 (pru0_1) | P9.22 / P9.21 / P9.18 / P9.17 | 0x09c / 0x0a0 / 0x0a4 / 0x074 | 4 |
| I2C2 (R5F): SCL/SDA = P9.19/P9.20 | | 0x208 / 0x20c | 4 |
| I2C4 (ARM): SCL/SDA = P9.24/P9.26 | | 0x1e0 / 0x1dc | 2 |

PRU GPO uses **mux mode 0** (output via `R30`), GPI **mode 1** (input via `R31`). `pru0_0` GPO pads:
`GPO2`=0x0b8 (P9.12), `GPO3`=0x0bc (P9.27), `GPO4`=0x0c0 (P9.15), `GPO5`=0x0c4 (P8.04),
`GPO8`=0x0d0 (P8.26), `GPO16-19`=0x0f0/0f4/0f8/0fc (P8.12/11/15/16).

---

## 5. Device-tree changes

In `device-tree/k3-j721e-boneai64-openWearable.dts`. Applied incrementally per §8.

### 5a. Pin groups (in `&main_pmx0`)
```dts
/* pru0_0 bus: McSPI7 on non-PRU timer/ecap pads */
main_spi7_pins_default: main-spi7-default-pins {
	pinctrl-single,pins = <
		J721E_IOPAD(0x234, PIN_OUTPUT, 6) /* (U3) SPI7_CLK  P9.31 */
		J721E_IOPAD(0x238, PIN_INPUT,  6) /* (V6) SPI7_D0   P9.30 MISO */
		J721E_IOPAD(0x23c, PIN_OUTPUT, 6) /* (V5) SPI7_D1   P9.29 MOSI */
		J721E_IOPAD(0x230, PIN_OUTPUT, 6) /* (U2) SPI7_CS0  P9.28 */
	>;
};

/* pru0_1 bus: McSPI6 on ICSSG1/PRG1 pads (controller-driven; ICSSG1 unused) — Step 2 */
main_spi6_pins_default: main-spi6-default-pins {
	pinctrl-single,pins = <
		J721E_IOPAD(0x09c, PIN_OUTPUT, 4) /* (AC22) SPI6_CLK  P9.22 */
		J721E_IOPAD(0x0a0, PIN_INPUT,  4) /* (AJ22) SPI6_D0   P9.21 MISO */
		J721E_IOPAD(0x0a4, PIN_OUTPUT, 4) /* (AH22) SPI6_D1   P9.18 MOSI */
		J721E_IOPAD(0x074, PIN_OUTPUT, 4) /* (AC21) SPI6_CS0  P9.17 */
	>;
};
```
I2C2 (R5F) and I2C4 (ARM) pin groups already exist (`main_i2c2_pins_default`,
`main_i2c4_pins_default`) and their nodes are already `status="okay"`. The `pru0_0`/`pru0_1` GPO
groups mux their 9/14 header pads to **mode 0** (attached to the ICSSG0 consumer or a `&main_pmx0`
hog) — **excluding** the three board-power pads above.

### 5b. Controller nodes
```dts
/* Step 1: enabled in Linux so spidev proves the wiring; pru0_0 then mmaps 0x2170000.
 * For PRU ownership this later flips to status="disabled" + A72 clock/firewall. */
&main_spi7 {
	status = "okay";
	pinctrl-names = "default";
	pinctrl-0 = <&main_spi7_pins_default>;
	#address-cells = <1>;
	#size-cells = <0>;
	spidev@0 { compatible = "rohm,dh2228fv"; reg = <0>; spi-max-frequency = <12000000>; };
};
/* &main_spi6 → same shape for pru0_1 (Step 2). */
```
✅ The modified DTB compiles clean (`DTC`, no warnings).

---

## 6. Driver port — `spidriver.c` → `mcspi_j721e.c`

The OMAP McSPI IP is **identical** AM335x↔J721E (`ti,omap4-mcspi`, regs_offset `0x100`), so
`library/pru/src/mcspi_j721e.{c,h}` is the proven AM335x `spidriver.c` with two deltas:

1. **base passed in** (no hard-coded `SOC_SPI_1_REGS`) — the same code drives McSPI7 (`0x2170000`)
   and McSPI6 (`0x2160000`); register offsets (`SYSCONFIG 0x110`, `CH0CONF 0x12C`, `TX0 0x138`, …)
   are unchanged.
2. **`CM_PER` clock-enable removed** — on J721E the functional clock is provisioned outside the
   firmware (Linux/TISCI).

`mcspiHlRev(base)` reads `MCSPI_HL_REV` (base+0x000) as the **firewall/clock smoke test** — the first
access a core makes; a sane non-zero value proves the master reached the register block.

> Verify the McSPI **functional clock** (AM335x = 48 MHz) on J721E and adjust `clkdiv` if it differs.

---

## 7. What's outside the DT (per owning core, once Linux is off it)

1. **Clock:** with the controller `disabled` in Linux, the A72 enables its functional clock via TISCI
   `set_device_state = ON` before the PRU/R5F drives it.
2. **Firewall:** confirm the **ICSSG0 PRU master-ID** (and `main_r5fss0` for I2C2) is admitted to the
   peripheral region — the #1 risk, answered by the `mcspiHlRev()` readback.
3. **Firmware:** the PRU mem-maps the McSPI base and calls `mcspiInit`/`mcspiXfer`; the R5F does the
   same for I2C2 (ported `i2cdriver.c`).

---

## 8. Modular bring-up steps (each gated by a Linux-side proof first)

- **Step 1 — `pru0_0` ↔ McSPI7** (`0x2170000`): (a) `spidev` loopback / device read proves the bus;
  (b) `mcspi_j721e.c` — **done**; (c) `pru0_0` firmware: `mcspiHlRev` smoke test → transfer;
  (d) cross-check vs `spidev_test`.
- **Step 2 — `pru0_1` ↔ McSPI6** (`0x2160000`): same pattern, second core/controller (uses CS0;
  CS1-3 spare).
- **Step 3 — R5F ↔ I2C2** (`0x2020000`): port `i2cdriver.c` (same base-swap), R5F mmaps, verify vs
  `i2cdetect -y 2`.
- **ARM ↔ I2C4** (`/dev/i2c-3`): already live in Linux — reserve + `i2cdetect -y 3`.

---

### Summary
- **Two large headers (P8+P9) are the whole pin field.** ICSSG0-only target.
- **pru0_0 → McSPI7, pru0_1 → McSPI6** — hardware SPI via mem-map, **0 PRG0 cost** (9 + 14 GPIO kept).
- **R5F → I2C2, ARM → I2C4** — the only two clean header I²C buses.
- Driver `mcspi_j721e.c` = base-swap of the proven AM335x McSPI driver. DTB compiles clean; Step 1
  awaits the on-board spidev proof + reboot.
