# tidl/vision-apps — TI SDK patch-overlay

> **Expert path.** For openWearable's *own* C7x inference use [`../ow/`](../ow/) — fleet-free, no
> fleet needed. Use this directory only when you need TI's full TIOVX / `vision_apps` runtime (e.g.
> the resnet OSRT proof). The two paths: [`../README.md`](../README.md). Full runbook:
> [`../../docs/C7X-TIDL.md`](../../docs/C7X-TIDL.md).

We patch a few small things in TI's PSDK-RTOS to boot the `vision_apps` firmware fleet on the
BeagleBone AI-64 against its stock boot firmware. TI's SDK source is multiple GB — we do **not**
vendor it. Instead this directory carries only the **recipe**:

```
tidl/vision-apps/
  MANIFEST.md          every TI component pinned to PSDK 10.01.00.04 (versions, sources)
  build-firmware.sh    clone/locate the pinned SDK → apply patches → build the 5 fleet ELFs
  patches/             small, reviewable .patch files + the spec for each (see patches/README.md)
  src/   (git-ignored) the actual TI SDK checkout — populated locally, never committed
  out/   (git-ignored) build outputs (the vx_app_rtos_linux_*.out ELFs)
```

`src/` and `out/` are git-ignored. The TI SDK itself lives wherever TI's installer put it
(default `~/ti`); point `build-firmware.sh` at it with `TI_SDK_HOME`.

## Why an overlay, not a fork

The actual patch surface is tiny — essentially **one ABI check** plus **one build flag** (the
rest of the bring-up lives in our device tree, already in-tree). A patch-overlay keeps the
change **diffable and code-reviewable** as a few lines of text, pins exact upstream versions,
and avoids carrying gigabytes of TI source or maintaining a parallel fork. If the patch surface
ever grows large enough to need its own history/CI, we can graduate to GitHub forks pinned as
submodules under `src/` — the manifest already records the exact base versions.

## Usage (x86_64 Linux build host)

```bash
# 1. Install the pinned TI components from MANIFEST.md (PSDK-RTOS 10.01.00.04 + the CGTs).
export TI_SDK_HOME=~/ti            # install root for PSDK-RTOS + compilers

# 2. Build the patched fleet.
tidl/vision-apps/build-firmware.sh   # → tidl/vision-apps/out/vx_app_rtos_linux_{mcu2_0,mcu2_1,c6x_1,c6x_2,c7x_1}.out

# 3. Deploy + run: see docs/C7X-TIDL.md §2.3–§4.
```

> These steps reproduce the firmware that is **proven working** on the board (resnet18 runs on
> the C7x, 52/52 nodes offloaded). The build runs on an x86_64 host because the C7x/C66
> compilers have no arm64 build. See `docs/C7X-TIDL.md` for the full picture.

## The fleet: 5 firmwares, one barrier

`vision_apps` will not start inference until **all six CPUs** clear an all-or-nothing sync barrier
— `mpu1_0 + mcu2_0 + mcu2_1 + c6x_1 + c6x_2 + c7x_1`. A single dead core stalls *everything*, so
even a pure-C7x graph needs the two R5Fs and two C66s alive. That is why this path is heavy — and
why it needs the **`-vision-apps` DTB** (split-mode R5F + both C66), **not** the base openWearable
DTB.

## Deploy (board)

Symlink each built ELF to the remoteproc's *derived* firmware name in `/lib/firmware`, then cold-boot:

| ELF (`out/`) | core | `/lib/firmware` symlink |
|---|---|---|
| `vx_app_rtos_linux_mcu2_0.out` | main_r5fss0 core0 | `j7-main-r5f0_0-fw` |
| `vx_app_rtos_linux_mcu2_1.out` | main_r5fss0 core1 | `j7-main-r5f0_1-fw` |
| `vx_app_rtos_linux_c6x_1.out`  | C66_0 | `j7-c66_0-fw` |
| `vx_app_rtos_linux_c6x_2.out`  | C66_1 | `j7-c66_1-fw` |
| `vx_app_rtos_linux_c7x_1.out`  | C7x   | `j7-c71_0-fw` |

`reboot` to load them (**never** warm-restart a vision R5F/DSP — it wedges the K3 fabric). The
step-by-step deploy + the resnet run: [`../../docs/C7X-TIDL.md`](../../docs/C7X-TIDL.md) §2.3–§4.
