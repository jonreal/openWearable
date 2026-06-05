# openWearable — Claude working notes

openWearable is an open-source **real-time hardware/software platform for wearable
robotics** (soft exosuits, powered ankle-foot prostheses, haptic feedback, EMG/IMU
sensing). It pairs a Linux application processor with bare-metal real-time cores on a
TI Sitara/Jacinto SoC, using a shared-memory + flow-control design to get
deterministic **>kHz control loops** alongside a full Linux userland. This low-level
real-time determinism is the project's core value proposition (and the reason for
this platform over, e.g., an NVIDIA Jetson).

## Active branches (BOTH are maintained)

| Branch    | Target board            | SoC                                   | Status                                   |
|-----------|-------------------------|---------------------------------------|------------------------------------------|
| `master`  | BeagleBone Black / Blue | TI **AM335x** (1× Cortex-A8 + 2× PRU) | Stable. Keep alive — do not break.       |
| `am64x`   | **BeagleBone AI-64**    | TI **TDA4VM / J721E**                  | **Active development** — heterogeneous compute. |

> ⚠️ The branch is named `am64x`, but its **real target is the TDA4VM (J721E)** on the
> BeagleBone AI-64 (application cores are **Cortex-A72**). The build naming is now honest:
> the firmware, the `-D` define, and the `.cmd` files are `j721e`, and the template builds
> `-mtune=cortex-a72`. The remaining `am64x` names are *intentional* — the **branch name**
> (rename deferred) and **`PSSP_ARCH = am64x`** (the PRU/ICSSG headers, which J721E shares).
> The remoteproc paths `j7-pru0_0` / `j7-pru0_1` in `pru.c` are correct for this SoC.

## The architecture pattern (shared by both branches)

Every app is assembled from a few per-core "hook" files plus a shared library. Cores
communicate through a **shared-memory region** guarded by **flow-control bits**; each
core runs an `Init / Update / Cleanup` lifecycle. The Linux side loads bare-metal
firmware onto the real-time cores via **remoteproc** (sysfs: stop → write firmware
name → start).

- `library/` — shared core. `include/` headers, `src/` Linux/A-core code,
  `pru_src/` PRU firmware. Built into `libowpru.a` **first**.
- `apps/<app>/` — one app = `state.h` (shared-mem layout), `pruloop.c` (PRU hooks),
  `cpuloop.c` (A-core compute), `uiloop.c` (menu + logging), `format.c` (log format),
  `Makefile`, `PRU*_<arch>.cmd` (linker scripts).
- `device-tree/` — per-board `.dts` (pinmux, memory carveouts, remoteproc config).
- `gui/`, `utils/`, `hardware/`, `scripts/`, `ros/`, `misc/` — tooling, PCBs, helpers.

### How the current `am64x` runtime works
- `library/src/main.c` (A72/Linux): parse args → `PruInit()` loads PRU firmware via
  remoteproc → `PruMemMap()` mmaps shared SRAM through `/dev/mem | O_SYNC`
  (**uncached**) → load params/LUT → `PruEnable()` → run `UiLoop()` → teardown.
- `library/src/pru.c`: remoteproc loader + shared-memory mapper. Currently hardcoded
  to the two PRU nodes.
- `library/pru_src/pru0_main.c`: IEP-timer-driven hard-real-time loop at `fs`
  (e.g. 1 kHz). PRU0 ↔ PRU1 run in lockstep via the `pru_ctl` done-bit handshake.
  Calls the app's `Pru{0,1}{Init,UpdateState,UpdateControl,Cleanup}` hooks.
- Shared types live in `apps/<app>/state.h` + `library/include/mem_types.h`. Everything
  shared is `volatile`; fixed memory offsets must match the `.cmd` linker files on
  both the host and PRU sides.

## Build & deploy

Development is on a laptop (this repo); **building and running happen on the board over
ssh** (sources live at `/root/openWearable`). Board: **`root@boneai64-0.local`** (key auth
is set up — no password needed). See `docs/milestone-1-hetero/BOARD-TRUTH.md` for the live
hardware inventory.
1. `cd library && make` → builds `libowpru.a` (+ Linux objects).
2. `cd apps/<app> && make` → builds the A-core binary (`gcc`) + PRU ELFs (`clpru`),
   installs firmware to `/lib/firmware/`, binary to `apps/bin/`.
3. Run: `apps/bin/<app>`  (flags: `-c <config>`, `-l <lut>`, `-v` debug, `-d` no-DMA,
   `-s` stats).
4. Device-tree changes require recompiling the `.dts` and rebooting the board.

## Current initiative: heterogeneous compute (Milestone 1)

The `am64x` branch is being extended from 2-tier (PRU + Linux) into a full
**heterogeneous mesh** across the TDA4VM:

- **2× Cortex-A72** — Linux orchestration / UI / logging.
- **R5F** (5 of 6 available; **MCU R5F core0 is reserved for device-manager/TIFS —
  do not touch**) — hard real-time control brain (1 GHz, FPU).
- **C7x + MMA** (~8 TOPS) — ML inference / heavy linear algebra.
- **2× C66x DSP** — classical DSP front-end (filtering, feature extraction).
- **2× PRU-ICSSG** — lowest-level deterministic I/O and µs timing.

First real app will be an **EMG pipeline running a lightweight, offline-trained
(PyTorch) transformer on the C7x**. **Milestone 1** is the foundation for it: a new
`template-hetero` app that brings up **one of each core type in unison** with (1) a
unified two-plane memory model, (2) a generalized multi-core remoteproc loader, and
(3) minimal heartbeat firmware per core. No NN yet — this is the scaffold.

📋 **Before working on this initiative, read `docs/milestone-1-hetero/`** — it holds the
*locked* architecture decisions (memory-coherence model, synchronization model) and the
detailed execution plan:
- `00-OVERVIEW.md` — locked decisions, core inventory, acceptance test, phase roadmap.
- `PHASE-0.md` — board ground-truth + toolchain setup (extremely detailed).
- `PHASE-1.md` — unified memory model (extremely detailed).

## Conventions & gotchas
- Shared structs are `volatile`. The **control plane is non-cacheable**, so `volatile`
  alone is sufficient (no cache maintenance). The **data plane is cacheable** and needs
  explicit writeback (producer) / invalidate (consumer) at hand-off.
- Memory offsets are duplicated across `memmap.h` / `state.h`, each core's linker
  `.cmd`, the device-tree carveout, and the ARM `mmap` — **always change them together.**
- A-cores on this board are **Cortex-A72** (not A53/A8); the template builds `-mtune=cortex-a72`.
- **Never repurpose MCU R5F core0** — it runs the device-manager/TIFS firmware that the
  whole system depends on.
