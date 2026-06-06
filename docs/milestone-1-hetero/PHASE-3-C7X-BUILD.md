# Phase 3 (C7x) — Build the TIDL stack from source → SDK 10.1 (transformer-ready)

Goal: get a working **C7x + MMA** inference path onto the **openWearable image** (no SDK-image
reflash) by building the TIDL runtime from source. Building lands us on **SDK 10.1**, which is
**transformer-capable** — so we skip the 9.2 detour and go straight to the version we ultimately
need. Source: `TexasInstruments-Sandbox/edgeai-osrt-libs-build` (Docker-automated).

## Build host decision: the x86 box, NOT the board

**Board resource survey (2026-06, boneai64):** RAM 3.6 GB + 4 GB swap; **disk only 5.0 GB free**
(10.4 GB root, 49% used); **2 cores**; Docker not installed. → **Insufficient** for a from-source
onnxruntime build (~10–15 GB + heavy link RAM). So **build on the x86 Linux box** via the repo's
**Case 2 (QEMU arm64 Docker)**. The board is **install + run only** (already prepped: `uv` +
Python 3.10 venv at `~/tidl-venv`).

> The x86 box is needed for **artifacts** anyway (model compile = x86 `tidl_tools`), so it does
> double duty: build the runtime (QEMU) **and** compile artifacts (native x86).
> Fastest alternative: an **aarch64 cloud VM** (e.g. Graviton) builds natively (no QEMU slowdown).

## Components (all SDK 10.1 / config.yaml `main`, version-consistent)
| Component | Version | How |
|---|---|---|
| `onnxruntime_tidl` (runtime wheel; adds `TIDLExecutionProvider`) | 1.15.0 (TI fork `tidl-1.15`) | **build** |
| `arm-tidl` libs: `libvx_tidl_rt.so`, `libtidl_onnxrt_EP.so`, `libtidl_tfl_delegate.so` | REL.PSDK 10.01.00.04 | **build** |
| vision-apps libs (**incl. the C7x firmware** + deps) | 10.01.00.04 | **prebuilt download** |
| tflite 2.12 / dlr 1.13 | — | build (optional — skip for ONNX path) |

## Steps

### B1 — build the runtime (x86 box, QEMU)
```bash
git clone https://github.com/TexasInstruments-Sandbox/edgeai-osrt-libs-build && cd edgeai-osrt-libs-build
sudo apt-get install -y qemu-user-static && ./qemu_init.sh        # arm64 emulation
docker pull --platform linux/arm64 arm64v8/debian:12.5
BASE_IMAGE=debian:12.5 ./docker_build.sh
BASE_IMAGE=debian:12.5 ./docker_run.sh                            # enter container
# --- in container ---
./vision_apps_libs_download.sh          # firmware + deps (10.01.00.04) -> ~/debian12.5-deps
./onnxrt_prepare.sh && ./onnxrt_build.sh    # -> onnxruntime_tidl-1.15.0-cp310-cp310-linux_aarch64.whl
./tidl_prepare.sh   && ./tidl_build.sh      # -> libvx_tidl_rt.so, libtidl_onnxrt_EP.so
./collect_libs.sh                           # gather outputs   (or just ./build_all.sh for all)
```
Key outputs: the `onnxruntime_tidl` **aarch64 wheel**, the `libtidl_onnxrt_EP` / `libvx_tidl_rt`
libs, and the vision-apps libs + **10.1 C7x firmware**.

### B2 — install on the board
- `uv pip install --python ~/tidl-venv/bin/python <onnxruntime_tidl-…aarch64.whl>` (Python 3.10 venv).
- Place `libvx_tidl_rt.so` / `libtidl_onnxrt_EP.so` + vision-apps libs → set `LD_LIBRARY_PATH` /
  `TIDL_TOOLS_PATH`.
- Install the **10.1 C7x firmware** → `/lib/firmware/` (replaces `j7-c71_0-fw.tisdk`).
- **Validate:** does the C7x come up under remoteproc with the 10.1 firmware on the board's
  `6.12` kernel? ← the key risk (see below).

### B3 — artifacts (x86 box, native)
```bash
git clone https://github.com/TexasInstruments/edgeai-tidl-tools && cd edgeai-tidl-tools
git checkout <10.1 tag>          # match config.yaml's PSDK 10.01
source ./scripts/setup/setup.sh  # x86 tidl_tools
# compile resnet18 (basic_example -c) -> model-artifacts/ ; scp to the board
```

### B4 — run on the board
Inference example in `--infer` mode → confirm `TIDLExecutionProvider` is active and the
**offload report** shows the net on the C7x.

## The one real risk: firmware ↔ kernel
The build gives 10.1 firmware, but the board's kernel is `6.12.17-ti-arm64-r33` (shipped with
9.02). If remoteproc can't load the 10.1 C7x firmware on that kernel, we also need a
10.1-aligned kernel — still `apt`/`dpkg` + reboot, **not** a Debian reflash. Validated at B2.

## Effort
Multi-GB, multi-hour Docker build (onnxruntime), all scripted, + the firmware/kernel validation.
Ends with a **transformer-ready C7x** on the openWearable image.
