# TIDL on the BeagleBone AI-64 — Build, Patches & Bring-up

> Status: **WORKING.** The full TI `vision_apps` fleet (2× R5F + 2× C66 + C7x + A72)
> reaches cpu-sync, and **resnet18 OSRT runs on the C7x returning a real result**
> (52/52 nodes offloaded; top-1 `warplane` on `airshow.jpg`). This file documents the
> whole path so it is reproducible.
>
> Branch: `am64x` · Board: `root@boneai64-0.local` (key auth) · Date first green: 2026-06-14.

---

## 0. TL;DR — what it took

Getting any NN onto the C7x means running TI's `vision_apps` firmware fleet, because the
**C7x's TIDL runtime is built on the vision_apps framework** and the framework will not
proceed until **all six CPUs** clear an all-or-nothing `appLogCpuSyncInit` barrier
(`mpu1_0 + mcu2_0 + mcu2_1 + c6x_1 + c6x_2 + c7x_1`). One dead core stalls everything.

Five classes of fix, all **config/topology — no boot-firmware (TIFS) swap**:

| # | Layer | Fix | Why |
|---|-------|-----|-----|
| 1 | Kernel | install prebuilt `6.12.57-ti-arm64-r64` (`CONFIG_REMOTEPROC_CDEV=y`) | userspace `/dev/remoteproc*` cdev the TIDL runtime needs |
| 2 | DTB | vision_apps memory carveouts (C7x, C66, IPC, dma-heap, per-core heaps) | remoteproc must map each firmware's PT_LOAD + runtime regions |
| 3 | DTB | reserve DSP tick-timers (`main_timer0/1/2`) from Linux | C7x/C66 FreeRTOS tick timers were A72-owned → scheduler hung |
| 4 | DTB | disable SA2UL crypto | `sa_sha_cra_exit` oops killed wifi on r64 kernel |
| 5 | DTB | **mcu2_0 carveout merge (31 MB)** | mcu2_0 fw's ~17 MB `.bss` must fit ONE PT_LOAD carveout |
| 6 | DTB | **split-mode `main_r5fss0` → mcu2_1 on 5d00000** + disable `main_r5fss1` | mcu2_1 was on the wrong physical core → Sciclient hung |
| 7 | Firmware | **relax `Sciclient_abiCheck`** (accept ACK'd resp) | board DM speaks TISCI ABI 3, PSDK-10.1 fw expects 4 |
| 8 | Firmware | **`BUILD_ENABLE_ETHFW=no`** for mcu2_0 | `appEthFwInit` hangs on the missing EVM GESI Ethernet card |

Fixes 1–4 are in the committed `device-tree/k3-j721e-boneai64-openWearable-tidl.dts`
and prior commits; 5–6 are the dts commit `1690c38`; 7–8 are firmware-source patches
rebuilt on the build host (sophon).

---

## 1. Baseline

- **SoC**: TI TDA4VM / J721E. A-cores = 2× Cortex-A72 (Linux). Accelerators: **C7x + MMA**
  (~8 TOPS, *the only core that runs TIDL/ONNX*), 2× C66x DSP (classical DSP, **no MMA, no
  TIDL**), 4× R5F (2 clusters of 2), 2× PRU-ICSSG.
- **PSDK pin**: `10.01.00.04` everywhere (firmware, TIDL runtime, edgeai-tidl-tools).
- **edgeai-tidl-tools SOC id**: `am68pa` (set `export SOC=am68pa`).
- **Kernel**: `6.12.57-ti-arm64-r64` (apt-installed; has `REMOTEPROC_CDEV`).
- **Boot firmware (TIFS/DM)**: board's stock `Fiery Fox 10.0.1`, **TISCI ABI 3.1**
  (`tiboot3.bin` + `sysfw.itb`, not reflashed — see fix #7).
- **A72-side TIDL runtime**: built on the board at `~/tidl`
  (`onnxruntime_tidl-1.15.0`, `libtidl_onnxrt_EP.so`, `libvx_tidl_rt.so`). Activate with
  `source ~/tidl/setenv.sh`. See `c7x-tidl-runtime` memory.

### Cores ↔ remoteproc ↔ firmware (the fleet)

| vision_apps CPU | phys node | `/sys/...remoteproc` | firmware symlink | ELF load base |
|-----------------|-----------|----------------------|------------------|---------------|
| mcu2_0 | `5c00000.r5f` (main_r5fss0_core0) | running | `j7-main-r5f0_0-fw` | `0xa2100000` |
| **mcu2_1** | **`5d00000.r5f` (main_r5fss0_core1)** | running | **`j7-main-r5f0_1-fw`** | `0xa4100000` |
| c6x_1 | `4d80800000.dsp` | running | `j7-c66_0-fw` | `0xa8100000` |
| c6x_2 | `4d81800000.dsp` | running | `j7-c66_1-fw` | `0xa9100000` |
| c7x_1 | `64800000.dsp` | running | `j7-c71_0-fw` | `0xb2100000` |
| (DM) | `41000000.r5f` | attached | — | — |

`main_r5fss1` (`5e00000`/`5f00000`, "mcu3") is **disabled** — not part of the fleet.
Firmware names are *derived* from the node (no `firmware-name` DT prop); all symlinks
point at `vx_app_rtos_linux_*.out.ow-debug` builds.

---

## 2. Why the whole fleet has to come up

The TIDL graph itself is **C7x-only** (resnet offloaded 52/52 to C7x; the C66/R5F do
nothing for it). But the A72 `libtivision_apps` runtime is **compiled** for the 6-CPU
fleet and calls `appLogCpuSyncInit`, which blocks until every CPU checks in. So even a
pure C7x inference needs all of mcu2_0/mcu2_1/c6x_1/c6x_2 alive and synced. That is the
*only* reason they matter here — it is a framework requirement, not a compute one. (This
is the key tension behind the architecture question in §9.)

---

## 3. The patches

### 3.1 Kernel — REMOTEPROC_CDEV  *(no rebuild)*
`apt-get install` the prebuilt `linux-image-6.12.57-ti-arm64-r64` deb (verified
`CONFIG_REMOTEPROC_CDEV=y`). Gives the `/dev/remoteproc*` char devices the TIDL runtime
opens. See `c7x-kernel-cdev-fix` memory.

### 3.2 DTB — vision_apps memory carveouts
In `reserved-memory` (see the dts, ~lines 45–298). Each remote core's firmware PT_LOADs
into a `shared-dma-pool` carveout; runtime heaps are plain `no-map` reservations. Layout
is contiguous `0xaa000000 → 0xd8000000` plus high-DDR for the C7x:

- `rtos_ipc_memory_region` `@aa000000` 32 MB — IPC vrings (`ipc_vring` links here).
- `rtos_log` `@ac000000` 256 KB — the shared per-core log (see §7).
- `tiovx_obj_desc` `@ac040000`, `app_fileio` `@b0000000`, `tiovx_log_rt` `@b0400000`.
- `c71_0_dma_tidl` `@b2000000` 1 MB + `c71_0_tidl` `@b2100000` 95 MB — C7x code+data.
- `vision_apps_shared-memories` `@b8000000` 512 MB — **dma-heap-carveout** (TIDL tensors;
  node name must be exactly `vision_apps_shared-memories`, no `@addr`, mappable/no `no-map`).
- per-core DDR heaps (NOBITS, runtime): mcu2_0 `@d9000000`, mcu2_1 `@da000000`,
  c6x_1 `@dc000000`, c6x_2 `@e0000000`.
- C7x high DDR (2nd 2 GB bank): scratch `@0x8_80000000` 368 MB, heap `@0x8_97000000` 256 MB.
- C66 firmware repointed `&c66_0`→`a8` and `&c66_1`→`a9` (the 10.1 c6x_1/c6x_2 ELFs link
  there); `&c71_0`→`b2`.

### 3.3 DTB — reserve DSP tick-timers
```dts
&main_timer0 { status = "reserved"; };   /* DM Timer 0 -> C66x_1 tick */
&main_timer1 { status = "reserved"; };   /* DM Timer 1 -> C66x_2 tick */
&main_timer2 { status = "reserved"; };   /* DM Timer 2 -> C7x_1  tick */
```
Stock Beagle DT binds Linux `omap_timer` to the entire main DMTimer bank, so the A72
owned DMTimer2; the C7x FreeRTOS `TimerP_create(DMTimer2)` then stalled (DM won't grant an
A72-owned timer) → scheduler never started. The A72 uses `arch_sys_counter` and needs
none of these. R5F ticks use MCU-domain timers 12/13 (different bank), left alone.

### 3.4 DTB — disable SA2UL crypto
```dts
&main_crypto { status = "disabled"; };
```
The r64 kernel's `iwd` AF_ALG hash teardown hits `sa_sha_cra_exit` → bad `dma_unmap` →
kills wifi at boot. The A72's ARMv8 crypto instructions take over; nothing in openWearable
uses SA2UL.

### 3.5 DTB — mcu2_0 carveout merge (31 MB)  *(commit 1690c38)*
mcu2_0's 10.1 firmware has a ~17 MB `.bss`; its PT_LOAD spans `a2100000..a3ffec00` (incl.
a 5 KB IRQ-stack block at the top, `0xa3ffec00`). remoteproc maps **one carveout per
PT_LOAD segment**, so this must be a single contiguous region. Merged the old
`15M + a3 pair` into one **31 MB** region at `a2100000` (`reg = <0 0xa2100000 0 0x1f00000>`)
and dropped the absorbed `main_r5fss0_core1_*` reserved-memory nodes.

### 3.6 DTB — split-mode `main_r5fss0`, mcu2_1 → 5d00000  *(commit 1690c38, the finale)*
**Root cause.** The dtsi default is **lockstep** (`ti,cluster-mode = <1>`), so
`main_r5fss0_core1` (`5d00000`) never registered a remoteproc — it existed as a platform
device but not a bootable core. mcu2_1's firmware therefore got loaded onto
`main_r5fss1_core0` (`5e00000`, the "mcu3_0" slot) via `j7-main-r5f1_0-fw`. But the
firmware is built `BUILD_MCU2_1`, whose Sciclient **host context is `MAIN_0_R5_NONSEC_1`**
(`pdk .../sciclient/soc/V1/sciclient_soc_priv.h`) — designed for `5d00000`. Running on the
wrong physical cluster, it drove the wrong secure-proxy TX/RX threads, the Device Manager
never answered, and **every Sciclient call hung `WAIT_FOREVER`** at `appSciclientInit`.
mcu2_1 never joined the barrier → fleet stalled → resnet timed out. (This is exactly the
A72's recurring `_rpmsg_char_find_rproc: 5d00000.r5f is not probed` — it expects mcu2_1
at `5d00000`.)

**Fix** (three edits):
```dts
&main_r5fss0 { ti,cluster-mode = <0>; };          /* split: 5d00000 becomes a remoteproc */

&main_r5fss0_core1 {                              /* mcu2_1, now on its native core */
    mboxes = <&mailbox0_cluster1 &mbox_main_r5fss0_core1>;   /* already correct */
    memory-region = <&main_r5fss1_core0_dma_memory_region>,  /* a4000000 dma  */
                    <&main_r5fss1_core0_memory_region>,      /* a4100000 load */
                    <&main_r5fss1_core1_memory_region>;      /* a5100000 IRQ-stack @a5ffec00 */
};

&main_r5fss1 { status = "disabled"; };            /* mcu3 not in fleet; frees a4/a5 of dual-claim */
```
mcu2_1's ELF links at `a4100000..a444ffff` + IRQ-stack PT_LOAD `@0xa5ffec00` + `ipc_vring
@0xaa000000` (readelf-confirmed) — a *fixed* link map independent of which core runs it,
so `5d00000` reuses the regions historically labelled `main_r5fss1_core0/core1`. mcu2_0 is
on the same physical cluster and already worked → the board boardcfg authorizes
`MAIN_0_R5`, so `5d00000` (ctx1, same cluster) is serviced with **no boot-firmware change**.

**Board-side companion** (not in-tree, needs go-ahead — see `board-changes-need-go-ahead`):
```sh
ln -sf vx_app_rtos_linux_mcu2_1.out.ow-debug /lib/firmware/j7-main-r5f0_1-fw
```
`5d00000` auto-requests `j7-main-r5f0_1-fw` (derived name).

### 3.7 Firmware — relax `Sciclient_abiCheck`  *(THE unblocker)*
File: `pdk .../drv/sciclient/src/sciclient.c`, `Sciclient_abiCheck()`. The board DM
(`Fiery Fox 10.0.1`) speaks **TISCI ABI major 3**; PSDK-10.1 Sciclient expects **4**
(`soc/V1/sciclient_fmwMsgParams.h SCICLIENT_FIRMWARE_ABI_MAJOR = 4U`). The transport
*works* (we proved it sends `TISCI_MSG_VERSION` and gets an ACK with `abi_major = 3`); only
the strict major check fails → `CSL_EFAIL` → `APP_ASSERT_SUCCESS` hangs `while(1)`. The skew
is cosmetic (R5F does UDMA + 10k DM ping-pongs fine on ABI 3).

**Fix**: drop the `(response.abi_major != SCICLIENT_FIRMWARE_ABI_MAJOR)` term so any ACK'd
version response passes (equivalently set the expected major to 3). Apply **for all cores**
(early attempts guarded it `BUILD_C7X || _TMS320C6X`, which left the R5Fs hung — make it
unconditional). Reusable BBAI64 board fix; see `c7x-abi-skew-fix` memory. (Alt "proper"
fix: swap `sysfw.itb` to PSDK-10.1 TIFS so the DM reports ABI 4 — deeper boot-chain risk.)

### 3.8 Firmware — disable ETHFW (mcu2_0)
`app_init.c` `appInit()` calls `appEthFwInit()` between `appMemInit` and `appIpcInit` under
`#ifdef ENABLE_ETHFW`, and **mcu2_0 is the ethfw core**. `appEthFwInit` →
`EthFwBoard_init(ETHFW_BOARD_GESI_ENABLE)` expects the EVM's GESI/QSGMII Ethernet card,
which the BBAI64 lacks → assert/hang. (This is why *only* the C7x cleared `appInit` before
the fix — it isn't the ethfw core.)

**Fix**: `export BUILD_ENABLE_ETHFW=no` before the firmware build
(`sdk_builder/vision_apps_build_flags.mak: BUILD_ENABLE_ETHFW ?= yes`; `?=` respects env).
**Gotcha**: TI makefiles don't track the DEFS flip → `rm` mcu2_0's `app_init.obj`,
`main.obj`, and `vx_app_rtos_linux_mcu2_0.out` to force the recompile (do **not** touch the
shared `app_init.c`, which would cascade a relink into the working C7x).

### 3.9 Debug-only residue (clean up later)
The *currently deployed* mcu2_1 firmware (`md5 f6ff1e94`) also carries a **GTC-skip** patch
(`app_utils/.../app_timer_rtos.c`: for `R5F`, hardcode `clkFreq = 200 MHz`, skip the
`Sciclient_pmGetModuleClkFreq(GTC0, WAIT_FOREVER)` query) **plus breadcrumb stores**. These
were diagnostics from before the core-placement fix. Now that mcu2_1 is on `5d00000` with a
working Sciclient, the GTC query would succeed on its own — a clean rebuild (abiCheck-relax
only, no GTC-skip, no breadcrumbs) is the production target. It works as-is, so this is
cleanup, not a blocker. See §8.

---

## 4. Building the firmware (on the build host `sophon.local`)

Toolchains in `~/ti` (cgt-armllvm 3.2.2 for R5F, cgt-c7000 4.1.0 for C7x, ti-cgt-c6000
8.3.7 for C66). PSDK RTOS source at `~/psdk-rtos/`.

```sh
cd sdk_builder
export LD_LIBRARY_PATH=$HOME/locallibs
export C6X_GEN_INSTALL_PATH=$HOME/ti/ti-cgt-c6000_8.3.7
export BUILD_ENABLE_ETHFW=no          # fix 3.8
# build all vision cores:
BUILD_EDGEAI=yes BUILD_CPU_C6x_1=yes BUILD_CPU_C6x_2=yes ./make_firmware.sh
```
- Output → `psdk_fw/j721e/vision_apps_evm/vx_app_rtos_linux_{mcu2_0,mcu2_1,c6x_1,c6x_2,c7x_1}.out`
  (stripped from `vision_apps/out/J721E/{R5F,C66,C71}/FREERTOS/release/`).
- The OS is **FreeRTOS** (not SafeRTOS): `out/J721E/C71/FREERTOS/...`.
- Per-CPU `BUILD_CPU_*` flags are env-overridable (`?=`). C66 build needs the c6000
  toolchain + `C6X_GEN_INSTALL_PATH`.
- After editing a *header* or a DEFS flag, `rm` the affected `.obj` (or `touch` the `.c`) —
  TI makefiles don't track those deps.

## 5. Deploying (sophon → laptop → board)

No direct sophon→board key, so hop through the laptop. On the board, install as
`*.ow-debug` and repoint the `j7-*` symlinks (stock `.tisdk` / `.10.01.00.04` kept as
rollback). **Firmware swaps + reboots need explicit go-ahead** (`board-changes-need-go-ahead`).

```sh
# on board, per core e.g. mcu2_1:
cp vx_app_rtos_linux_mcu2_1.out  /lib/firmware/vx_app_rtos_linux_mcu2_1.out.ow-debug
ln -sf vx_app_rtos_linux_mcu2_1.out.ow-debug /lib/firmware/j7-main-r5f0_1-fw
# DTB:
scp ...-tidl.dts root@board:/root/openWearable/device-tree/      # symlinked into the DT tree
ssh root@board 'cd ~/BeagleBoard-DeviceTrees && make install_arm64'   # -> /boot/firmware/ti/*.dtb
ssh root@board 'reboot'   # cores auto-boot from the new DTB; reboots are fine unattended
```
> ⚠️ **Never warm-restart (stop→start) a vision R5F core** — it hard-hangs the K3 fabric
> (power-cycle required). Always cold-boot via reboot. **Never touch the MCU R5F core0
> (`41000000.r5f` / TIFS).**

Validate a DTB edit *without* installing (pure build):
```sh
cd ~/BeagleBoard-DeviceTrees
cpp -nostdinc -Iinclude -Isrc/arm64 -Isrc -Itestcase-data -undef -D__DTS__ \
    -x assembler-with-cpp -o /tmp/ow.pre src/arm64/ti/k3-j721e-boneai64-openWearable-tidl.dts
dtc -@ -O dtb -o /tmp/ow.dtb -i src/arm64/ti -Wno-unit_address_vs_reg /tmp/ow.pre
```

## 6. Running inference (the resnet proof)

Artifacts (TIDL-compiled) ship in `~/edgeai-tidl-tools-10/model-artifacts/cl-ort-resnet18-v1_4batch/`.
**Use the `~/tidl` venv** (`onnxruntime_tidl`, has `TIDLExecutionProvider`) — *not*
`edgeai-tidl-tools/myenv` (stock onnxruntime, no TIDL EP).

```sh
source /root/tidl/setenv.sh          # activates ~/tidl/venv + sets TIDL_TOOLS_PATH/LD_LIBRARY_PATH
export SOC=am68pa
cd /root/edgeai-tidl-tools-10/examples/osrt_python/ort
python3 onnxrt_ep.py -m cl-ort-resnet18-v1_4batch
```
Expected: `libtidl_onnxrt_EP loaded`, `Offloaded Nodes 52/52`, top-1 `warplane, military
plane`, `IPC: Init ... Done` with **no** "Unable to create TX channels" errors, exit 0.

## 7. Debug toolbox

- **Per-core boot log (best tool, no serial)**: every core writes `appLogPrintf` into a slot
  of `APP_LOG_MEM @ 0xAC000000` (non-cacheable, so A72 `/dev/mem` reads see it directly).
  Slot = `base + cpu_idx*0x4000`. cpu_idx: `MPU1_0=0, DM=1, mcu2_0=3, mcu2_1=4, c6x_1=7,
  c6x_2=8, c7x_1=9`. Per-slot: `+0x00 rd_idx, +0x04 wr_idx, +0x08 valid(==0x01357231),
  +0x0C sync_state, +0x10 name[16], +0x20 text`. Reader: `mmap("/dev/mem", O_SYNC,
  offset=0xAC000000)` in Python (`dd` gets EFAULT; mmap works). The C7x clobbers the shared
  base region, so for C66 use their slots above or per-core tracebufs.
- **Breadcrumbs**: store magic words to unused slot-15 scratch (`0xAC03Fxxx`) around a
  suspect call, read after a reboot; fresh DRAM = `0xFFFFFFFF`. Use a readback/fence after
  each store (an unfenced non-cacheable store can be lost if the core then stalls on a
  device access). Verify the store actually linked with `dis7x`/`readelf`.
- **Firmware load map**: `readelf -l <fw>.out | grep LOAD` → the PT_LOAD `PhysAddr` ranges
  drive carveout sizing. `readelf -S | grep -i 'resource\|tracebuf\|vring'` for IPC regions.
- **Topology**: `/sys/class/remoteproc/*/{state,firmware}` + `readlink device`; the R5F
  `firmware-name` is derived (`j7-main-r5fX_Y-fw` for `main_r5fssX_coreY`).

## 8. Known issues / cleanup TODO

- **mcu2_0 DSS-init abort (cosmetic, non-blocking).** Now that the fleet syncs, mcu2_0 runs
  *past* the barrier and aborts in the **display (DSS)** pipeline:
  `[FATAL] Core has Aborted DFAR=0xa010000 DFSR=0x808 (write)` after `DSS: Board init Done`.
  `0xa010000` is in the MCU-R5F dma region (outside mcu2_0's carveout) — a display
  DMA/pointer into an unmapped region, newly exposed because nothing reached DSS before.
  **Does not affect resnet** (TIDL graph is C7x-only; mcu2_0's rpmsg endpoint registers
  during `appIpcInit`, before DSS, so the A72 still creates its IPC channel). Fix later by
  building mcu2_0 **without DSS** (we don't drive a display) or sizing a DSS carveout.
- **Deployed firmware is debug-flavoured.** mcu2_1 = `f6ff1e94` (abiCheck + GTC-skip +
  breadcrumbs); others carry residual breadcrumb stores. Rebuild a clean production set:
  abiCheck-relax (all cores) + ethfw-off (mcu2_0), **no** GTC-skip, **no** breadcrumbs,
  **no** DSS on mcu2_0. Re-deploy + re-verify resnet.
- **Rollbacks on board**: DTB `…-tidl.dtb.preSplit-bak`; firmware `*.ethfw-bak`,
  `*.preBC-bak`, `*.abi-bak`, stock `*.tisdk` / `*.10.01.00.04`.

## 9. Big-picture architecture (open design question)

We now have **two proven C7x firmware strategies** for the EMG transformer:

- **(A) TI vision_apps RTOS + TIDL OSRT** — what runs today. Pros: TIDL runtime "just works",
  ONNX→C7x offload, batteries included. Cons: the heavyweight 6-CPU fleet + cpu-sync barrier
  + rpmsg/virtio IPC + per-core carveouts; lots of framework we don't need; the DSS/ethfw/
  abiCheck friction above. The C66/R5F only run because `libtivision_apps` demands the fleet.
- **(B) Bare-metal C7x** — proven viable: breadcrumbs showed our custom C runs through
  `main()` with working MMU/cache/DDR **before** `OS_start()`. A bare-metal core that never
  enters the RTOS scheduler sidesteps the entire fleet/Sciclient/IPC stack. Cons: we'd own
  the TIDL invocation (link `libvx_tidl_rt` directly, or hand-write/trim kernels) and the
  A72↔C7x handoff ourselves.

The **IPC/memory model** is the crux you flagged:
- vision_apps uses **rpmsg + virtio rings** for control and a **two-plane memory model**
  (control plane non-cacheable so `volatile` suffices; data plane cacheable with explicit
  writeback/invalidate at hand-off). General, but heavy.
- openWearable's existing PRU pattern is **shared-memory + flow-control bits** — lighter,
  deterministic, no rings. This is the project's core value proposition (CLAUDE.md).

**The decision** (for the heterogeneous-mesh Milestone 1): adopt TI's rpmsg/vision_apps IPC
for the C7x, or extend the lean shared-memory + flow-control model (the PRU loop) up to the
C7x/R5F? And which cores carry which role:
- **PRU** — lowest-level deterministic I/O (EMG ADC sampling, µs timing). Unambiguous.
- **C7x + MMA** — the transformer / heavy linear algebra. Unambiguous (only TIDL-capable core).
- **R5F vs "C7x + PRU only"** — the real fork. R5F gives a 1 GHz FPU hard-real-time control
  brain that already speaks the K3 IPC fabric; but bringing it up dragged in the whole fleet.
  If the C7x goes bare-metal (B), do we still want R5F in the loop for control, or can
  PRU(I/O) + C7x(compute) + A72(orchestration) cover Milestone 1 without R5F's framework cost?

> Recommendation to revisit after studying the code: prototype **(B) bare-metal C7x** with the
> **shared-memory + flow-control** plane (reuse the PRU design) for the EMG datapath, and keep
> the working vision_apps/TIDL fleet (A) as the reference + fallback. That keeps the
> deterministic core value while leaning on TI only for the C7x's TIDL kernels.

## 10. Quick reference

```
APP_LOG_MEM        0xAC000000  (slots 0x4000; idx: mpu1_0=0 dm=1 mcu2_0=3 mcu2_1=4 c6x_1=7 c6x_2=8 c7x_1=9)
breadcrumb scratch 0xAC03Fxxx  (slot 15)
DSP tick timers    main_timer0=DMTimer0->c6x_1  main_timer1->c6x_2  main_timer2->c7x   (all "reserved")
firmware symlinks  j7-main-r5f0_0-fw=mcu2_0  j7-main-r5f0_1-fw=mcu2_1  j7-c66_0-fw=c6x_1  j7-c66_1-fw=c6x_2  j7-c71_0-fw=c7x
run resnet         source ~/tidl/setenv.sh; export SOC=am68pa; cd ~/edgeai-tidl-tools-10/examples/osrt_python/ort; python3 onnxrt_ep.py -m cl-ort-resnet18-v1_4batch
build fw (sophon)  cd sdk_builder; export LD_LIBRARY_PATH=$HOME/locallibs C6X_GEN_INSTALL_PATH=$HOME/ti/ti-cgt-c6000_8.3.7 BUILD_ENABLE_ETHFW=no; BUILD_EDGEAI=yes BUILD_CPU_C6x_1=yes BUILD_CPU_C6x_2=yes ./make_firmware.sh
install dtb        cd ~/BeagleBoard-DeviceTrees && make install_arm64   (dts symlinked into src/arm64/ti/)
```

*See also the `memory/` notes: `c7x-needs-vision-apps-fleet` (full bring-up narrative),
`c7x-abi-skew-fix`, `c7x-tidl-runtime`, `c7x-kernel-cdev-fix`, `board-changes-need-go-ahead`.*
