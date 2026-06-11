# Phase 3 (C7x) — TIDL stack: status & road ahead (SDK 10.1, transformer-ready)

Goal: a working **C7x + MMA** inference path on the **openWearable Debian image** (no SDK reflash),
at **SDK 10.1** — the transformer-capable version we ultimately need. Reproducible build/install
steps for humans: board runtime + firmware in `../SETUP.md` Part F; **x86 model-artifact compile in
`TIDL-COMPILE-HOST.md`**. This file is the engineering status + plan.

## Status (2026-06-11)
- ✅ **A72 TIDL runtime built from source + installed** → `~/tidl` on the board. `TIDLExecutionProvider`
  verified live.
- ✅ **C7x firmware installed + booted** (10.01.00.04). `/lib/firmware/j7-c71_0-fw` → the binary;
  `remoteproc5` `state=running`, `virtio1` rpmsg online. Stable, no crash/recovery.
- ✅ **DTB carveout gate CLOSED.** `device-tree/k3-j721e-boneai64-openWearable-tidl.dts` ports TI's full
  vision_apps `reserved-memory` map; built, deployed, rebooted — all 10 regions reserved cleanly.
- 🟡 **Per-model artifacts: x86 compile PROVEN** (2026-06-11) — resnet18 → **52/52 nodes on C7x**,
  "Subgraph Compiled Successfully", on the `sophon` x86 host. Full recipe + gotchas in
  `TIDL-COMPILE-HOST.md`. ⬜ On-board inference run (load artifacts via `~/tidl`) — **next**.

## What we actually did (supersedes the original "build on the x86 box / QEMU" plan)
Built the runtime **natively on the board**, using an SD card for space (the eMMC + 3.6 GB RAM can't
host an onnxruntime build). The full procedure + the one critical CMake gotcha
(`CMAKE_POLICY_VERSION_MINIMUM=3.5`) are in `../SETUP.md` Part F. Result in `~/tidl`: the
`onnxruntime_tidl` cp311 wheel + `libtidl_onnxrt_EP.so` / `libvx_tidl_rt.so` / `libtidl_tfl_delegate.so`
/ `libtivision_apps.so`, all PSDK 10.01.00.04. Verified:
`onnxruntime.get_available_providers()` → `['TIDLExecutionProvider','TIDLCompilationProvider','CPUExecutionProvider']`.

> Note: `TIDLCompilationProvider` is present in the on-board runtime — we may be able to **compile
> model artifacts on-target**, sidestepping the x86 host for step B3. Untested.

## The three pieces of TIDL (do not conflate them)
| Piece | Runs on | Built / obtained | Status |
|---|---|---|---|
| A72 runtime libs + wheel | A72 / Linux | built from source (SETUP Part F) | ✅ `~/tidl` |
| **C7x firmware** `vx_app_rtos_linux_c7x_1.out` | the C7x core (remoteproc-loaded) | **prebuilt download** | ✅ staged |
| per-model artifacts `*.bin` | loaded *into* the firmware at runtime | `edgeai-tidl-tools` (x86 or on-target) | ⬜ |

remoteproc handles transport, but it still must load the **firmware** (the actual TIDL engine) onto
the C7x — the A72 libs are useless without it. The firmware is **model-agnostic**; the per-model
`.bin` is loaded at runtime like a serialized TensorRT engine. "Binaries built on the Linux box" =
the per-model artifacts, **not** the firmware.

## C7x firmware — source (the clean standalone one)
- Repo `git.ti.com/cgit/processor-sdk/psdk_fw`, tag **`10.01.00.04`**,
  file `j721e/vision_apps_eaik/vx_app_rtos_linux_c7x_1.out` (**12,574,864 B**, C7x ELF, machine 0x91).
- `wget "https://git.ti.com/cgit/processor-sdk/psdk_fw/plain/j721e/vision_apps_eaik/vx_app_rtos_linux_c7x_1.out?h=10.01.00.04"`
- Same dir has the C66x firmware (`vx_app_rtos_linux_c6x_1.out`) for later. The `main` branch is a
  newer SDK (~11.x) if we ever bump versions.
- remoteproc5 (`64800000.dsp`) asks for the firmware name `j7-c71_0-fw`; the board keeps cores offline
  by suffixing firmware `.tisdk`; enable by symlinking `j7-c71_0-fw` → the firmware. Stock 9.02
  firmware stays as `j7-c71_0-fw.tisdk` (fallback).

## ✅ The gate (CLOSED): DTB `reserved-memory` now matches the 10.1 J721E map
Static ELF inspection of the 10.1 firmware vs the board's *stock* DTB carveouts had been:

| Firmware (10.1) load region | Stock DTB carveout | match? |
|---|---|---|
| `0xb2100000`–`~0xb2ec0000` (code/data + `.resource_table`) | `c71-memory@a8100000` | ❌ wrong address |
| `0xaa000000` (IPC vrings) | `ipc-memories@aa000000` (28 MB) | ⚠️ too small (needs 32 MB) |
| `0xac000000`, `0xac040000`, `0xb0000000` (log / tiovx / file) | — | ❌ unreserved |

The 9.02 firmware loaded *inside* `c71-memory@a8100000` (entry `0xa8e00000` ✓). The 10.1 firmware is
hard-linked to `0xb2100000`, which was **general Linux RAM** on the stock board — so remoteproc refused it.

**Fix (done):** `device-tree/k3-j721e-boneai64-openWearable-tidl.dts` — a byte-copy of the board DTS
with only the C7x carveouts changed: grew `ipc-memories` 28→32 MB, added TI's full vision_apps map
(`rtos-log`, `tiovx-obj-desc` 63.75 MB, `app-fileio`, `tiovx-log-rt`, `c71` dma@b2000000 + mem@b2100000
95 MB, `ddr-shared` 512 MB, high-DDR `c7x-scratch` 368 MB + `c7x-heap` 256 MB), and repointed `&c71_0`
A8→B2. Old A8 c71 nodes kept (orphaned); R5F/C66x heaps + inter-core eth omitted. Built/deployed via
SETUP Part C, rebooted. dmesg confirms all 10 regions `nomap non-reusable`, no overlap; the rproc
driver bound to `c71-dma-memory@b2000000`. Annotated reference map:
`j721e-visionapps-reserved-memory.dts`. **Device-tree change only** — kernel/Debian untouched,
reversible via `extlinux.conf`.

## Road ahead
1. ~~**DTB carveouts**~~ ✅ **DONE (2026-06-09)** — `-tidl.dts` deployed, C7x boots (`state=running`,
   `virtio1` rpmsg online). See SETUP Part F.4 for the boot/verify steps.
2. ~~**Artifacts (B3)**~~ ✅ **x86 compile DONE (2026-06-11)** — `edgeai-tidl-tools` on `sophon`,
   resnet18 → 52/52 on C7x. Recipe in `TIDL-COMPILE-HOST.md`. (Same flow will compile the EMG net.)
3. **Inference (B4, next):** copy `subgraph_0_tidl_*.bin` to the board, run via `~/tidl` with
   `TIDLExecutionProvider`; confirm the on-device offload report puts the net on C7x+MMA.
4. **Mesh integration:** wire into the two-plane memory model (`INFERENCE-DATAFLOW.md`) — A72 publishes
   `intent` into the non-cached control plane for the PRU/R5F loop.

## Artifacts on the board (this save point)
- Runtime install: `~/tidl/` (`lib/` + `venv/` + `setenv.sh`).
- Build tree (SD): `/mnt/build/edgeai-osrt-libs-build/workarea/`; deps `~/debian12.5-deps/`.
- C7x firmware: **installed** at `/lib/firmware/vx_app_rtos_linux_c7x_1.out.10.01.00.04` with
  `/lib/firmware/j7-c71_0-fw` → it (auto-loads on reboot). Staging copy still on SD at
  `/mnt/build/fw/...` (sha256 `d8dc6d98df30769b70d00a195556386e78a369c014c9bf9d022eb236935a5e31`).
- Device tree: `device-tree/k3-j721e-boneai64-openWearable-tidl.dts`, selected in
  `/boot/firmware/extlinux/extlinux.conf` (`eMMC (default)` label).
