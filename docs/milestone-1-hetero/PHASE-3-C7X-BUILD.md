# Phase 3 (C7x) — TIDL stack: status & road ahead (SDK 10.1, transformer-ready)

Goal: a working **C7x + MMA** inference path on the **openWearable Debian image** (no SDK reflash),
at **SDK 10.1** — the transformer-capable version we ultimately need. Reproducible build/install
steps for humans live in `../SETUP.md` Part F; this file is the engineering status + plan.

## Status (2026-06-07)
- ✅ **A72 TIDL runtime built from source + installed** → `~/tidl` on the board. `TIDLExecutionProvider`
  verified live.
- ✅ **C7x firmware found, verified, staged** (10.01.00.04) → `/mnt/build/fw/`. **Not installed/booted.**
- 🔴 **Gate: device-tree carveout mismatch** blocks the C7x boot — a DTB update + reboot is required.
- ⬜ Per-model artifacts (compile a net) + on-board inference — not started.

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

## 🔴 The gate: DTB `reserved-memory` must match the 10.1 J721E map
Static ELF inspection of the 10.1 firmware vs the board's live DTB carveouts:

| Firmware (10.1) load region | Board DTB carveout | match? |
|---|---|---|
| `0xb2100000`–`~0xb2ec0000` (code/data + `.resource_table`) | `c71-memory@a8100000` | ❌ wrong address |
| `0xaa000000` (IPC vrings) | `ipc-memories@aa000000` (28 MB) | ✅ |
| `0xac000000`, `0xac040000`, `0xb0000000` (log / tiovx / file) | — | ❌ unreserved |

The 9.02 firmware loaded *inside* `c71-memory@a8100000` (entry `0xa8e00000` ✓). The 10.1 firmware is
hard-linked to `0xb2100000`, which is **general Linux RAM** on this board, not a `no-map` carveout —
so remoteproc refuses it.

**Fix:** port TI's 10.1 J721E `reserved-memory` carveouts into
`device-tree/k3-j721e-boneai64-openWearable.dts`, rebuild + deploy (SETUP Part C), reboot. This is a
**device-tree change only** — kernel/Debian untouched, reversible via `extlinux.conf`, and (per the
project lead) inherently kernel-independent. It folds into the Milestone-1 memory-model pass that
adds `ow_ctrl`/`ow_data` in the same DTS.

## Road ahead
1. **DTB carveouts (next):** get TI's 10.1 J721E `reserved-memory` map (matching kernel DTS), port
   into our DTS, rebuild/reboot, then `echo start > /sys/class/remoteproc/remoteproc5/state` and read
   the remoteproc log to confirm the C7x boots.
2. **Artifacts (B3):** compile resnet18 (then our EMG net) → `.bin`. Try on-target via
   `TIDLCompilationProvider` first; fall back to `edgeai-tidl-tools` on an x86 box.
3. **Inference (B4):** run on the C7x; confirm the offload report puts the net on C7x+MMA.
4. **Mesh integration:** wire into the two-plane memory model (`INFERENCE-DATAFLOW.md`) — A72 publishes
   `intent` into the non-cached control plane for the PRU/R5F loop.

## Artifacts on the board (this save point)
- Runtime install: `~/tidl/` (`lib/` + `venv/` + `setenv.sh`).
- Build tree (SD): `/mnt/build/edgeai-osrt-libs-build/workarea/`; deps `~/debian12.5-deps/`.
- C7x firmware (SD): `/mnt/build/fw/vx_app_rtos_linux_c7x_1.out.10.01.00.04`
  (sha256 `d8dc6d98df30769b70d00a195556386e78a369c014c9bf9d022eb236935a5e31`).
