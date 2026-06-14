# TI component manifest — pinned versions

Everything below is **pinned to the PSDK 10.01.00.04 (10.1.0 GA) line.** The firmware, the A72
runtime libs, and the model-compile tools must all be the same 10.1.0 version or the TISCI/ABI
and IPC layers skew. Do not mix point releases (`10_01_03_00` / `10_01_04_00` are *later* 10.1.x
releases — wrong for this board).

| Component | Version / tag | Source | Used for |
|---|---|---|---|
| **PSDK-RTOS J721E** | `10.01.00.04` | TI processor-sdk-rtos-j721e installer | firmware fleet build (vision_apps, pdk, app_utils) |
| **C7000 CGT** (`cl7x`) | `4.1.0.LTS` | <https://www.ti.com/tool/C7000-CGT> | C7x firmware |
| **C6000 CGT** (`cl6x`) | `8.3.7` | <https://www.ti.com/tool/C6000-CGT> | C66x firmware (`C6X_GEN_INSTALL_PATH`) |
| **TI ARM CGT-Clang** (`tiarmclang`) | `3.2.2` | TI | R5F firmware |
| **Linux kernel (board)** | `6.12.57-ti-arm64-r64` | apt (`linux-image-6.12.57-ti-arm64-r64`) | `CONFIG_REMOTEPROC_CDEV=y` for `/dev/remoteproc*` |
| **A72 runtime libs** | `arm-tidl-j721e_10.1.0`, `onnxruntime_tidl-1.15.0`, `libtivision_apps.so.10.1.0` | `edgeai-osrt-libs-build` @ `main` (==10.01.00.0x) | board-side TIDL/ONNX runtime (`~/tidl`) |
| **edgeai-tidl-tools** | tag `10_01_00_02`, **SOC `am68pa`** | <https://github.com/TexasInstruments/edgeai-tidl-tools> | per-model `.bin` artifact compile (x86) |
| **Prebuilt C7x firmware** (fallback) | `10.01.00.04` | `git.ti.com/.../psdk_fw/j721e/vision_apps_eaik/` | smoke-test the C7x core without a full SDK build |

## Boot firmware — intentionally NOT changed

The board keeps its **stock boot firmware**: TIFS/Device-Manager **`Fiery Fox 10.0.1`, TISCI
ABI 3.1** (`tiboot3.bin` + `sysfw.itb`, as flashed by the Debian image). We do **not** reflash
it. The PSDK-10.1 firmware expects TISCI ABI major 4; rather than touch the boot chain, the
overlay relaxes the firmware-side ABI check (see `patches/`). This keeps the whole change
firmware + device-tree only and fully reversible.

## What the bring-up changes, and where each lives

Only the **firmware** rows are patches to TI source. The device tree is openWearable's *own*
file (authored by us — not a patch against TI's); the kernel is a stock apt install.

| Layer | Change | Lives in | Tracked? |
|---|---|---|---|
| Device tree (ours) | vision_apps carveouts; DSP tick-timer reserve; SA2UL disable; mcu2_0 31MB merge; R5F split-mode | `device-tree/k3-j721e-boneai64-openWearable-tidl.dts` | ✅ in-tree |
| Kernel | REMOTEPROC_CDEV | apt install (no patch) | n/a |
| Firmware (TI patch) | `Sciclient_abiCheck` ABI-major relax | `third_party/ti/patches/` + `build-firmware.sh` | ✅ recipe in-tree |
| Firmware (TI flag) | `BUILD_ENABLE_ETHFW=no` (mcu2_0) | `third_party/ti/build-firmware.sh` | ✅ in-tree |

Debug-only diagnostics from bring-up (a GTC-clock-query skip and breadcrumb stores) are
**deliberately excluded** from this production overlay — they were artifacts of debugging the
core-placement bug and are unnecessary now that mcu2_1 boots on its native core.
