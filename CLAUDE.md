# openWearable — Claude working notes

openWearable is an open-source **real-time hardware/software platform for wearable
robotics** (soft exosuits, powered ankle-foot prostheses, haptic feedback, EMG/IMU
sensing). It pairs a Linux application processor with bare-metal real-time cores on a
TI Sitara/Jacinto SoC, using a shared-memory + flow-control design to get
deterministic **>kHz control loops** alongside a full Linux userland. This low-level
real-time determinism is the project's core value proposition (and the reason for
this platform over, e.g., an NVIDIA Jetson).

## Active branches (per-target)

| Branch   | Target board            | SoC                                   | Status                                          |
|----------|-------------------------|---------------------------------------|-------------------------------------------------|
| `main`   | BeagleBone Black / Blue | TI **AM335x** (1× Cortex-A8 + 2× PRU) | Stable — public/product, minimal.               |
| `lab`    | BeagleBone Black        | TI **AM335x**                         | Research apps off `main` (additive only).       |
| `am64x`  | **BeagleBone AI-64**    | TI **TDA4VM / J721E**                 | **This branch.** Active dev — heterogeneous compute. |

> The branches are kept **separate on purpose** (the A72/R5F/C7x/C66/PRU-ICSSG mesh is
> AM335x-impossible), but the shared library backends are kept **near-identical** so fixes
> port cleanly. `pru.c` is byte-identical to main's except `PruMemMap` (the memory model).

> ⚠️ This branch is named for the PRU/ICSSG header package (`PSSP_ARCH = am64x`), but its
> **real target is the TDA4VM (J721E)**: firmware, `-D` define, and `.cmd` files are `j721e`,
> A-cores are **Cortex-A72** (`-mtune=cortex-a72`), and the remoteproc nodes are
> `j7-pru0_0`/`j7-pru0_1`.

## The architecture pattern (shared by both targets)

Every app is assembled from a few per-core "hook" files plus a shared library. Cores
communicate through a **shared-memory region** guarded by **flow-control bits**; each
core runs an `Init / Update / Cleanup` lifecycle. The Linux side loads bare-metal
firmware onto the real-time cores via **remoteproc** (sysfs: stop → write firmware
name → start).

- `library/` — shared core, split per execution context:
  - `common/` — cross-core contract (`mem_types.h`, `fix16`, integer types).
  - `a72/` — Cortex-A72 / Linux app-core code (`a8/` on main). `board.c` holds the
    per-SoC remoteproc node paths + firmware prefix (`extern` in `pru.h`).
  - `pru/` — PRU firmware. Built into `libow_pru.a` **first**.
- `apps/<app>/` — one app = `state.h` (shared-mem layout + memory-map constants),
  `pruloop.c` (PRU hooks), `cpuloop.c` (A-core compute), `uiloop.c` (menu + logging +
  `PruLoadParams`), `format.c` (log schema), `Makefile`, `PRU{0,1}_j721e.cmd`.
- `device-tree/` — per-board `.dts`. `gui/`, `utils/`, `hardware/`, `scripts/` — tooling.

### How the `am64x` (J721E) runtime works
- `library/a72/src/main.c` (A72/Linux): parse args → `PruMemMap()` mmaps the ICSSG shared
  RAM → `PruLoadParams()`/`PruLoadLut()` → `PruInit()` loads firmware via remoteproc (and
  `PruWaitRunning` blocks until both PRUs report `running`, closing the memInit/pru_ctl
  cold-boot race) → `PruEnable()` → `UiLoop()` → teardown.
- `library/a72/src/pru.c`: remoteproc loader (`/dev/remoteproc/j7-pru0_{0,1}` from `board.c`;
  firmware `j721e-pru{0,1}-<app>-fw`) + **unified single-mmap** shared-RAM mapper —
  `global_sharedram_base` with `param`/`lut` at offsets within it (vs main's three separate
  PRU0/PRU1/shared-RAM maps; `PruMemMap` is the only function that differs from main).
- `library/pru/src/pru0_main.c`: IEP-timer hard-real-time loop at `fs`. PRU0 ↔ PRU1 lockstep
  via the `pru_ctl` done-bit handshake; calls the app's `Pru{0,1}{Init,UpdateState,
  UpdateControl,Cleanup}` hooks.
- Shared types: `apps/<app>/state.h` (+ memory-map consts) + `library/common/include/mem_types.h`.
  Everything shared is `volatile`; fixed offsets must match the `.cmd` linker files.

### Logging
- The A-core logger packs **fixed-size binary records** behind a self-describing ASCII
  header (`#openWearable-log v1` / `#fields:` / `#record_bytes:` / `#DATA`); `format.c` owns
  the per-app schema. A real-time `SIGALRM` callback packs records into a lock-free SPSC RAM
  ring; a writer thread drains it to the file with `write()`, so disk latency stays off the
  control loop and run length is unbounded. Host decoder: `utils/logdecode/`. `log.c`/`log.h`
  are byte-identical to main's (proven on boneblack; am64x runtime re-validation pending).

## Build & deploy

Development is on a laptop (this repo); **building and running happen on the board over
ssh** (sources at `/root/openWearable`).

**Repo docs split:** `docs/` = public, reproducible setup manuals (no personal hostnames).
`notes/` = internal planning + live board/host inventory — **git-ignored, never published**.
`third_party/ti/` = the TI SDK **patch-overlay** (pinned manifest + patches that reproduce the
C7x/vision_apps firmware without vendoring TI's multi-GB source).

1. `cd library && make` → builds `libow_pru.a`.
2. `cd apps/<app> && make` → A-core binary (`gcc`, `-mtune=cortex-a72`) + PRU ELFs (`clpru`);
   installs firmware to `/lib/firmware/`, binary to `apps/bin/`.
3. Run: `apps/bin/<app>` (flags: `-c <config>`, `-l <lut>`, `-v` debug, `-d` no-DMA — logging
   no-op, `-s` stats).
4. Device-tree changes require recompiling the `.dts` and rebooting the board.

## Current initiative: heterogeneous compute (Milestone 1)

The `am64x` branch is being extended from 2-tier (PRU + Linux) into a full
**heterogeneous mesh** across the TDA4VM:

- **2× Cortex-A72** — Linux orchestration / UI / logging.
- **R5F** (5 of 6 available; **MCU R5F core0 is reserved for device-manager/TIFS — do not
  touch**) — hard real-time control brain (1 GHz, FPU).
- **C7x + MMA** (~8 TOPS) — ML inference / heavy linear algebra.
- **2× C66x DSP** — classical DSP front-end (filtering, feature extraction).
- **2× PRU-ICSSG** — lowest-level deterministic I/O and µs timing.

First real app: an **EMG pipeline running a lightweight, offline-trained (PyTorch) transformer
on the C7x**. **Milestone 1** is the foundation: a `template-hetero` app bringing up **one of
each core type in unison** with (1) a unified two-plane memory model, (2) a generalized
multi-core remoteproc loader, and (3) minimal heartbeat firmware per core. No NN yet.

📋 **Before working on this initiative, read `notes/milestone-1-hetero/`** (local, not in the
public tree) — locked architecture decisions (memory-coherence + synchronization model) and the
execution plan: `00-OVERVIEW.md`, `PHASE-0.md` (board ground-truth + toolchain), `PHASE-1.md`
(unified memory model).

## Conventions & gotchas
- Shared structs are `volatile`. The **control plane is non-cacheable**, so `volatile` alone
  suffices. The **data plane is cacheable** and needs explicit writeback (producer) / invalidate
  (consumer) at hand-off across A72/C7x/C66.
- Memory offsets are duplicated across `state.h` / `mem_types.h`, each core's `.cmd`, the
  device-tree carveout, and the ARM `mmap` — **always change them together.**
- A-cores are **Cortex-A72**; the template builds `-mtune=cortex-a72`. (`PruSprintMalloc`'s `%i`
  for `sizeof` warns on 64-bit — harmless; a shared `%zu` fix should be applied to both branches'
  `pru.c` to keep them identical.)
- Per-SoC values (`rp_pru0/rp_pru1/pru_fw_prefix`) live in `board.c` (`extern` in `pru.h`);
  `PruLoadParams` is a per-app hook in each app's `uiloop.c`.
- **Never repurpose MCU R5F core0** (device-manager/TIFS), and never warm-restart a vision R5F.
