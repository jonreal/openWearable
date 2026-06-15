# openWearable — Claude working notes

openWearable is an open-source **real-time hardware/software platform for wearable
robotics** (soft exosuits, powered ankle-foot prostheses, haptic feedback, EMG/IMU
sensing). It pairs a Linux application processor with bare-metal real-time cores on a
TI Sitara/Jacinto SoC, using a shared-memory + flow-control design to get
deterministic **>kHz control loops** alongside a full Linux userland. This low-level
real-time determinism is the project's core value proposition (and the reason for
this platform over, e.g., an NVIDIA Jetson).

## Active branches (BOTH are maintained)

| Branch   | Target board            | SoC                                   | Status                                          |
|----------|-------------------------|---------------------------------------|-------------------------------------------------|
| `main`   | BeagleBone Black / Blue | TI **AM335x** (1× Cortex-A8 + 2× PRU) | Stable — primary release target. Keep minimal.  |
| `am64x`  | **BeagleBone AI-64**    | TI **TDA4VM / J721E**                 | **Active development** — heterogeneous compute. |

> The default branch was renamed `master` → `main` (AM335x). `origin/master` is kept as a
> tombstone. The two branches are kept **separate on purpose**: the SoCs are too different
> (single A8 + PRU vs. an A72/R5F/C7x/C66/PRU-ICSSG mesh) to share one tree, but the shared
> library backends are kept **near-identical** so fixes port cleanly. Build/validate `main`
> on the AM335x board first.

> ⚠️ The `am64x` branch is named for the PRU/ICSSG header package (`PSSP_ARCH = am64x`), but
> its **real target is the TDA4VM (J721E)**: the firmware, `-D` define, and `.cmd` files are
> `j721e`, A-cores are **Cortex-A72**, and the remoteproc nodes are `j7-pru0_0`/`j7-pru0_1`.

## The architecture pattern (shared by both branches)

Every app is assembled from a few per-core "hook" files plus a shared library. Cores
communicate through a **shared-memory region** guarded by **flow-control bits**; each
core runs an `Init / Update / Cleanup` lifecycle. The Linux side loads bare-metal
firmware onto the real-time cores via **remoteproc** (sysfs: stop → write firmware
name → start).

- `library/` — shared core, split per execution context:
  - `common/` — cross-core contract (`mem_types.h`, `fix16`, integer types).
  - `a8/` — Cortex-A8 / Linux app-core code (`a72/` on am64x).
  - `pru/` — PRU firmware. Built into `libow_pru.a` **first**.
- `apps/<app>/` — one app = `state.h` (shared-mem layout), `pruloop.c` (PRU hooks),
  `cpuloop.c` (A-core compute), `uiloop.c` (menu + logging + `PruLoadParams`),
  `format.c` (log schema), `Makefile`, `PRU{0,1}_am335x.cmd` (linker scripts).
- `test/<test>/` — integration tests, same shape as an app but built to **`test/bin`**
  (off `$PATH`) with a test-specific firmware name. See `test/README.md`.
- `device-tree/` — per-board `.dts` (the live AM335x board is
  `am335x-boneblack-openWearable.dts`). `gui/`, `utils/`, `hardware/`, `scripts/` — tooling.

### How the `main` (AM335x) runtime works
- `library/a8/src/main.c` (A8/Linux): parse args → `PruMemMap()` mmaps the PRU shared
  SRAM → `PruLoadParams()` / `PruLoadLut()` write the param/LUT regions → `PruInit()`
  loads PRU firmware via remoteproc → `UiInit()` → `PruEnable()` → `UiLoop()` → teardown.
- `library/a8/src/pru.c`: remoteproc loader (`remoteproc1`/`remoteproc2` = PRU0/PRU1,
  firmware `am335x-pru{0,1}-<app>-fw`) + the shared-SRAM mapper (base `0x4A300000` +
  fixed offsets). `dma.c` is a standalone EDMA3 backend (HAL); the logger does **not** use it.
- `library/pru/src/pru0_main.c`: IEP-timer-driven hard-real-time loop at `fs`
  (e.g. 1 kHz). PRU0 ↔ PRU1 run in lockstep via the `pru_ctl` done-bit handshake, calling
  the app's `Pru{0,1}{Init,UpdateState,UpdateControl,Cleanup}` hooks.
- Shared types live in `apps/<app>/state.h` + `library/common/include/mem_types.h`.
  Everything shared is `volatile`; fixed offsets must match the `.cmd` linker files.

### Logging
- The A-core logger packs **fixed-size binary records** behind a self-describing ASCII
  header (`#openWearable-log v1` / `#fields:` / `#record_bytes:` / `#DATA` sentinel);
  `format.c` owns the per-app schema. A real-time `SIGALRM` callback (producer) packs
  records into a lock-free **SPSC RAM ring**; a **writer thread** drains it to the file
  with `write()`, so disk/SD latency stays off the control loop and run length is unbounded.
  Backpressure = counted drops, never a stall. Host decoder: `utils/logdecode/` (binary → CSV).
- Benchmark: `test/logbench` ramps `fs` to find the sustainable logging rate. On AM335x the
  limit is **~18 kHz**, bounded by the fixed ~200 Hz A8 drain of the 96-deep PRU state-ring
  (`STATE_BUFF_LEN`) — **not** disk (drops stay 0 well past it).

## Build & deploy

Development is on a laptop (this repo); **building and running happen on the board over
ssh** (sources live at `/root/openWearable`).

**Repo docs split:** `docs/` = public, reproducible setup manuals (no personal
hostnames/paths). `notes/` = internal planning / board+host inventory — **git-ignored,
never published**. `third_party/ti/` = the TI SDK **patch-overlay** (am64x-only: pinned
manifest + patches that reproduce the C7x/vision_apps firmware without vendoring TI's source).

1. `cd library && make` → builds `libow_pru.a`.
2. `cd apps/<app> && make` → A-core binary (`gcc`) + PRU ELFs (`clpru`); installs firmware
   to `/lib/firmware/`, binary to `apps/bin/`.
3. Run: `apps/bin/<app>` (flags: `-c <config>`, `-l <lut>`, `-v` debug, `-d` no-DMA — now a
   logging no-op, `-s` stats).
4. Tests: `cd test/<test> && make` → `test/bin/`. `test/logbench/run_logbench.sh` sweeps the
   logging benchmark.
5. Device-tree changes require recompiling the `.dts` and rebooting the board.

## Current state — minimal public release

`main` was refactored to the per-core layout above (mirroring am64x), pruned to the
`template` app, and given the binary ring/thread logger + `test/` infra. The goal is a
**minimal, clean foundation for a public release** (template + one or two examples — keep
it small). Deferred: port am64x's unified single-mmap base+offset memory model +
`PruWaitRunning` into `pru.c`.

## Conventions & gotchas
- Shared structs are `volatile`. On `main`/AM335x (single A8 + PRU shared SRAM) `volatile`
  suffices. On `am64x` the **cacheable data plane** needs explicit writeback (producer) /
  invalidate (consumer) at hand-off across A72/C7x/C66.
- Memory offsets are duplicated across `state.h` / `mem_types.h`, each core's `.cmd`, the
  device-tree carveout, and the ARM `mmap` — **always change them together.**
- A-core on this board is **Cortex-A8**; the template builds `-mtune=cortex-a8
  -march=armv7-a+fp`. The PRU toolchain needs `-I$(CGT_DIR)/include` for `clpru`'s stdint.h.
- `PruLoadParams` lives in each app's `uiloop.c` as a per-app hook — an app/test that swaps
  `uiloop.c` (e.g. `test/logbench`) must re-supply it.
- Commit/push only when asked. Starting/stopping the PRU cores is routine here; on `am64x`
  **never** touch MCU R5F core0 (TIFS/device-manager) or warm-restart a vision R5F.
