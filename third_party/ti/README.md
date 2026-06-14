# third_party/ti — TI SDK patch-overlay

We patch a few small things in TI's PSDK-RTOS to boot the `vision_apps` firmware fleet on the
BeagleBone AI-64 against its stock boot firmware. TI's SDK source is multiple GB — we do **not**
vendor it. Instead this directory carries only the **recipe**:

```
third_party/ti/
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
third_party/ti/build-firmware.sh   # → third_party/ti/out/vx_app_rtos_linux_{mcu2_0,mcu2_1,c6x_1,c6x_2,c7x_1}.out

# 3. Deploy + run: see docs/C7X-TIDL.md §2.3–§4.
```

> These steps reproduce the firmware that is **proven working** on the board (resnet18 runs on
> the C7x, 52/52 nodes offloaded). The build runs on an x86_64 host because the C7x/C66
> compilers have no arm64 build. See `docs/C7X-TIDL.md` for the full picture.
