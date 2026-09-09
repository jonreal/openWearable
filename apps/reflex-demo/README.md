# reflex-demo

Equilibrium-point / impedance control of an **antagonist PAM pair**, with a
load-adaptive **reflex** and live control from the terminal, a browser, or a phone.

## Control model

Two potentiometers set a virtual spring; a UI-mutable `Pmax` scales it:

| Input        | Meaning                              | Range   |
|--------------|--------------------------------------|---------|
| `pot1` (ch0) | **EP** — equilibrium point `θ_d`     | −1 … +1 |
| `pot2` (ch1) | **stiffness** `k` (co-contraction)   |  0 … 1  |

```
stiff = k · Pmax
ep    = θ_d · stiff
Pd1   = clamp(stiff + ep, 0, Pmax)      # PAM 1 setpoint (zero load)
Pd2   = clamp(stiff − ep, 0, Pmax)      # PAM 2 setpoint (zero load)
```

The **EP controller** (`epcontroller.c`) owns this pure law. The **reflex**
(`reflex.c`) is a separate load-adaptive layer: while ON, it senses the pressure
differential and **jumps the equilibrium by `dP`** to unload the joint, relaxing
back toward the EP when the load is removed. Both edit `pam->pd`; the PAM's
HOLD-gate serializes them.

## Hardware (BeagleBone Black, AM335x)

- **i2c2** (SDA P9.20, SCL P9.19) → PCA9548A mux @ `0x70`; Honeywell HSC sensors @ `0x28`.
  Reservoir = mux ch6, PAM1 = ch5, PAM2 = ch4 (round-robin, 1 read/tick).
- **Valves** (PRU1 R30): PAM1 in P8.27 / out P8.28; PAM2 in P8.29 / out P8.30. N.C.
- **Pots** → ADC ch0 (EP) and ch1 (stiffness). `fs = 100 Hz`.

## Build & run

```sh
cd apps/reflex-demo && make          # -> apps/bin/reflex-demo + firmware to /lib/firmware
apps/bin/reflex-demo                 # local TUI
apps/bin/reflex-demo -u <hub-ip>     # + stream binary telemetry to a host
```

## Commands

### Run flags
| Flag        | Effect                                             |
|-------------|----------------------------------------------------|
| `-u <host>` | stream binary telemetry to `<host>:1500`           |
| `-v`        | verbose debug screen (no TUI)                      |
| `-c <file>` | config · `-l <file>` LUT · `-s` stats · `-d` no-DMA |

### Terminal (TUI) menu
| Key | Action              |   | Key | Action              |
|-----|---------------------|---|-----|---------------------|
| `s` | start log (name it) |   | `f` | set reflex threshold |
| `d` | stop + save log     |   | `r` | toggle reflex on/off |
| `m` | set `Pmax`          |   | `e` | exit                |
| `g` | set `dP` (reflex jump) | | | |

### Command channel (host → board, UDP :1501, ASCII `"<name> <value>\n"`)
Always listening; **gated by a single ARM switch — nothing applies unless armed.**

| Command            | Effect                                    |
|--------------------|-------------------------------------------|
| `arm 1` / `arm 0`  | **the gate** — enable / disable changes   |
| `Pmax <kPa>`       | pressure ceiling                          |
| `dP <val>`         | reflex jump size                          |
| `threshold <val>`  | reflex trigger threshold                  |
| `reflex 1` / `reflex 0` | reflex on / off                      |

### Web console
```sh
python -m gui.webhub.server --app reflex-demo --udp 1500 --http 8080 --board <board-ip>
# open http://<hub-ip>:8080  → live plots + sliders (Pmax/dP/threshold) + reflex toggle + ARM
```
The browser auto-disarms on disconnect; every command is confirmed by the telemetry echo.

## Telemetry / scope

Framed-binary UDP on :1500. Fields: `frame p_res p1m_raw p1m p1d u1 p2m_raw p2m p2d u2
trigger pot1 pot2 pmax dp thr reflex armed`. View as columns:

```sh
python gui/scope.py --port 1500
```

## Files
`pruloop.c` (PRU control) · `uiloop.c` (TUI + `PruLoadParams` + `CmdApply`) ·
`format.c` (log/telemetry schema) · `state.h` (shared-mem layout + `CMD_REFLEX`) ·
`gui/dashboard.py` (web console layout). Shared: `library/pru/src/{epcontroller,reflex,pam}.c`.
