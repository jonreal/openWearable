# Running TIDL / vision_apps on the C7x (BeagleBone AI-64)

How to get neural-network inference running on the **C7x + MMA** deep-learning accelerator of
the TDA4VM/J721E, on the stock BeagleBoard.org Debian image (no full SDK reflash). The proof
target is the **resnet18 OSRT example returning a real classification on the C7x**.

> **Prereqs.** Finish [SETUP.md](SETUP.md) Parts A–C first (flashed image, ssh, toolchains,
> and the openWearable device tree built + selected). This guide is **BeagleBone AI-64 only**.
> Everything is pinned to **PSDK 10.01.00.04** — the firmware, the A72 runtime libs, and the
> model-compile tools must all be the same 10.1.0 version or you will chase ABI ghosts.

---

## 0. The mental model: TIDL is three separate pieces

Keep these straight or you will debug the wrong layer:

| Piece | Runs on | Where it comes from |
|---|---|---|
| **A72 runtime libs + Python wheel** (`onnxruntime_tidl`, `libtidl_onnxrt_EP.so`, `libvx_tidl_rt.so`, `libtivision_apps.so`) | A72 / Linux | built from source on the board — [Part 1](#1-a72-runtime-libraries) |
| **vision_apps firmware fleet** (`vx_app_rtos_linux_*.out` for mcu2_0, mcu2_1, c6x_1, c6x_2, c7x_1) | the R5F / C66 / C7x cores, via remoteproc | built from the TI SDK with our patch-overlay — [Part 2](#2-the-vision_apps-firmware-fleet) |
| **Per-model artifacts** (`subgraph_*_tidl_net.bin`) | loaded *into* the C7x firmware at runtime | compiled from ONNX on an x86 host — [Part 3](#3-compiling-a-model-artifact-x86-host) |

The firmware is the inference engine; the A72 libs are useless without it; the `.bin` artifact
is the compiled network (≈ a serialized TensorRT engine), loaded at runtime. **All three must
be the same 10.1.0 version.**

### Why the *whole fleet* — not just the C7x

The C7x's TIDL runtime is built on TI's `vision_apps` framework, and that framework will not
proceed until **all six CPUs** clear an all-or-nothing barrier (`appLogCpuSyncInit`):
`mpu1_0 + mcu2_0 + mcu2_1 + c6x_1 + c6x_2 + c7x_1`. A single dead core stalls *everything* —
so even a pure-C7x inference needs the two R5Fs and two C66s alive and synced. The TIDL graph
itself is C7x-only (resnet offloads 52/52 nodes to the C7x); the other cores run purely to
satisfy `libtivision_apps`. That framework cost is the central tension behind openWearable's
own lean shared-memory design (see the architecture notes).

### Cores ↔ remoteproc ↔ firmware

| vision_apps CPU | phys node | firmware symlink | ELF load base |
|---|---|---|---|
| mcu2_0 | `5c00000.r5f` (main_r5fss0_core0) | `j7-main-r5f0_0-fw` | `0xa2100000` |
| mcu2_1 | `5d00000.r5f` (main_r5fss0_core1) | `j7-main-r5f0_1-fw` | `0xa4100000` |
| c6x_1 | `4d80800000.dsp` | `j7-c66_0-fw` | `0xa8100000` |
| c6x_2 | `4d81800000.dsp` | `j7-c66_1-fw` | `0xa9100000` |
| c7x_1 | `64800000.dsp` | `j7-c71_0-fw` | `0xb2100000` |
| (device manager) | `41000000.r5f` | — TIFS, **never touch** — | — |

Firmware names are *derived* from the node (no `firmware-name` DT property): a core at
`main_r5fssX_coreY` requests `j7-main-r5fX_Y-fw`. `main_r5fss1` ("mcu3", `5e00000`/`5f00000`)
is **disabled** — not part of the fleet.

---

## 1. A72 runtime libraries

Build the A72-side TIDL/ONNX runtime from source. This is the same on every board.

### 1.1 Build storage (the build needs ~15 GB + swap; eMMC has neither)

Use an SD card:
```bash
lsblk                                   # identify the SD — NOT mmcblk0 (= eMMC)
mkfs.ext4 /dev/mmcblk1 && mkdir -p /mnt/build && mount /dev/mmcblk1 /mnt/build
fallocate -l 6G /mnt/build/swap && chmod 600 /mnt/build/swap && mkswap /mnt/build/swap && swapon /mnt/build/swap
mkdir -p /mnt/build/docker              # point Docker at the SD: "data-root":"/mnt/build/docker"
#   in /etc/docker/daemon.json, then:   systemctl restart docker
```

### 1.2 Build the libs (Docker, from TI's sandbox repo)
```bash
cd /mnt/build && git clone https://github.com/TexasInstruments-Sandbox/edgeai-osrt-libs-build
cd edgeai-osrt-libs-build               # config.yaml `main` == PSDK 10.01.00.0x
BASE_IMAGE=debian:12.5 ./docker_build.sh
BASE_IMAGE=debian:12.5 ./docker_run.sh  # enter the container, then:
export CMAKE_POLICY_VERSION_MINIMUM=3.5  # ⚠️ CRITICAL — see note
./build_all.sh                          # downloads deps + builds (~7–8 h on the board)
```
> ⚠️ **The gotcha that stops you cold:** onnxruntime 1.15 vendors an old `date` lib whose
> `cmake_minimum_required` is rejected by the container's CMake 4.x ("Compatibility with CMake
> < 3.5 has been removed"). Export `CMAKE_POLICY_VERSION_MINIMUM=3.5` **before** building or the
> configure step dies in ~16 s.

Outputs (under `…/workarea/` and `~/debian12.5-deps/`):
- `onnxruntime_tidl-1.15.0-cp311-cp311-linux_aarch64.whl` (cp311 = the board's native Python 3.11)
- `arm-tidl-j721e_10.1.0-…tar.gz` → `libvx_tidl_rt.so.1.0`, `libtidl_onnxrt_EP.so.1.0`, `libtidl_tfl_delegate.so.1.0`
- `libti-vision-apps-j721e_10.1.0-debian12.5.deb` → `libtivision_apps.so.10.1.0`

### 1.3 Install the runtime to `~/tidl`
```bash
mkdir -p ~/tidl/lib                     # place the 4 .so libs, each with its bare .so symlink:
#   libvx_tidl_rt.so -> .so.1.0     libtidl_onnxrt_EP.so -> .so.1.0
#   libtidl_tfl_delegate.so -> .so.1.0   libtivision_apps.so -> .so.10.1.0
python3 -m venv ~/tidl/venv && source ~/tidl/venv/bin/activate
pip install <…>/onnxruntime_tidl-1.15.0-cp311-cp311-linux_aarch64.whl numpy pillow pyyaml
cat > ~/tidl/setenv.sh <<'EOF'
export TIDL_HOME=$HOME/tidl
export LD_LIBRARY_PATH=$TIDL_HOME/lib:${LD_LIBRARY_PATH}
export TIDL_TOOLS_PATH=$TIDL_HOME/lib
source $TIDL_HOME/venv/bin/activate
EOF
```
**Verify:**
```bash
source ~/tidl/setenv.sh
python -c "import onnxruntime as o; print(o.get_available_providers())"
# -> ['TIDLExecutionProvider', 'TIDLCompilationProvider', 'CPUExecutionProvider']
```
If you instead see only `AzureExecutionProvider`/`CPUExecutionProvider`, you are in the wrong
venv (stock onnxruntime). The `TIDLExecutionProvider` only exists in the `~/tidl` venv built above.

---

## 2. The vision_apps firmware fleet

The fleet firmware is **not** what TI ships in the stock Debian image — the stock `.tisdk`
firmware keeps the cores offline, and the 10.1 SDK build needs several patches to boot on this
board against its stock boot firmware. The patches are tiny and fully captured as a
**patch-overlay** in [`../third_party/ti/`](../third_party/ti/): a pinned manifest + `.patch`
files + a build driver. You build the TI SDK once, overlaid; the multi-GB SDK source stays out
of this repo.

### 2.1 What the bring-up needs, and where each piece lives

Booting the fleet takes changes at three layers — but only the **firmware** ones are actual
*patches* to TI source. The device-tree changes are openWearable's **own** device tree (not a
patch against anything), and the kernel is a stock apt install. All of it is config/topology —
**no boot-firmware (TIFS) swap.**

**1. openWearable's custom device tree** —
[`device-tree/k3-j721e-boneai64-openWearable-tidl.dts`](../device-tree/k3-j721e-boneai64-openWearable-tidl.dts)
(our file; already in-tree):

| Provides | Why |
|---|---|
| vision_apps memory carveouts (C7x/C66/IPC/dma-heap/per-core heaps) | remoteproc must map each firmware's PT_LOAD + runtime regions |
| reserved DSP tick-timers (`main_timer0/1/2`) | C7x/C66 FreeRTOS tick timers were A72-owned → scheduler hung |
| SA2UL crypto disabled (`&main_crypto`) | `sa_sha_cra_exit` oops killed wifi on the r64 kernel |
| mcu2_0 31 MB carveout merge; `main_r5fss0` split-mode so mcu2_1 boots on `5d00000`; `main_r5fss1` disabled | one carveout per PT_LOAD; mcu2_1 was on the wrong physical core → Sciclient hung |

**2. Kernel** — stock apt install, no patch: `linux-image-6.12.57-ti-arm64-r64`
(`CONFIG_REMOTEPROC_CDEV=y`, for the `/dev/remoteproc*` cdev the runtime needs).

**3. TI firmware patch-overlay** — [`third_party/ti/`](../third_party/ti/) (the *only* actual
patches to TI source):

| Patch | Why |
|---|---|
| relax `Sciclient_abiCheck` (accept the ACK'd response) | board DM speaks TISCI ABI 3, PSDK-10.1 fw expects 4 |
| `BUILD_ENABLE_ETHFW=no` (mcu2_0 build flag) | `appEthFwInit` hangs on the missing EVM GESI Ethernet card |

> **The two firmware fixes, in one line each.** (6) `Sciclient_abiCheck()` in the SDK's
> `sciclient.c` rejects the board's ABI-major 3 because PSDK-10.1 hardcodes 4; the transport
> works fine on ABI 3, so we drop the strict-major term (apply to **all** cores — guarding it to
> the DSPs leaves the R5Fs hung). (7) mcu2_0 is the ethfw core and `EthFwBoard_init` expects an
> EVM Ethernet card the AI-64 lacks, so we build mcu2_0 with ETHFW off.

### 2.2 Build the fleet (x86_64 host)

The C7x/C66 compilers (`cl7x`, `cl6x`) ship x86_64-only — there is no arm64 build, so the fleet
is cross-built on a Linux PC and the ELFs copied to the board. Install the TI PSDK-RTOS 10.01.00.04
and its compilers per the manifest, then:

```bash
export TI_SDK_HOME=~/ti                 # wherever TI's installer put the PSDK + CGTs
third_party/ti/build-firmware.sh        # clones/pins SDK pieces, applies patches, builds the 5 ELFs
```
See [`third_party/ti/README.md`](../third_party/ti/README.md) for the exact toolchain versions
and build flags. Outputs are the 5 `vx_app_rtos_linux_*.out` ELFs.

### 2.3 Deploy to the board

Copy each ELF to `/lib/firmware/` under a non-stock name and repoint the derived `j7-*` symlink
(keep the stock `.tisdk` as a rollback):

```bash
# per core, e.g. mcu2_1:
cp vx_app_rtos_linux_mcu2_1.out  /lib/firmware/vx_app_rtos_linux_mcu2_1.out.ow
ln -sf vx_app_rtos_linux_mcu2_1.out.ow /lib/firmware/j7-main-r5f0_1-fw
# (repeat: mcu2_0->j7-main-r5f0_0-fw, c6x_1->j7-c66_0-fw, c6x_2->j7-c66_1-fw, c7x_1->j7-c71_0-fw)
reboot                                  # cores cold-boot from the new firmware + DTB
```

> ⚠️ **Never warm-restart (stop→start) a vision R5F/DSP core** — it hard-hangs the K3 fabric and
> needs a power cycle. Always cold-boot via `reboot`. **Never touch the MCU R5F core0
> (`41000000.r5f`)** — it runs TIFS/the device manager that the whole SoC depends on.

### 2.4 Verify the fleet is up
```bash
for r in /sys/class/remoteproc/remoteproc*; do printf '%s ' "$(cat $r/firmware)"; cat $r/state; done
# expect j7-main-r5f0_0-fw, j7-main-r5f0_1-fw, j7-c66_0-fw, j7-c66_1-fw, j7-c71_0-fw all = running
```
Each core writes its boot log into a slot of the shared `rtos-log@ac000000` carveout; a healthy
fleet shows every CPU reaching `APP: Syncing with 5 CPUs ... Done`. (A small reader is in
[Part 5](#5-debugging-toolbox).)

---

## 3. Compiling a model artifact (x86 host)

The per-model `.bin` is compiled with TI's `edgeai-tidl-tools` in a container on an x86 Linux
box (same place as the fleet build is fine). Pin the tool to the **same 10.1.0 line** as the
board: tag `10_01_00_02`, **SOC `am68pa`** (TI's name for the TDA4VM/J721E).

```bash
git clone https://github.com/TexasInstruments/edgeai-tidl-tools.git ~/edgeai-tidl-tools
cd ~/edgeai-tidl-tools && git checkout 10_01_00_02
# Build TI's container (FROM ubuntu:22.04 → Python 3.10, matches the cp310 OSRT wheels), then:
docker exec -e SOC=am68pa <container> bash -c "cd /home/root && source ./setup.sh"  # downloads am68pa tidl_tools
docker exec <container> pip3 install onnx_graphsurgeon                              # onnxOpt dep
```

Compile with a **minimal options dict** (TI's `onnxrt_ep.py` injects am69a-only options that
fail EP init on am68pa). A self-contained `compile.py`:

```python
import os, glob, shutil, numpy as np, onnxruntime as rt
from PIL import Image

art = "/home/root/r18_artifacts"
shutil.rmtree(art, ignore_errors=True)        # artifacts_folder MUST be empty, else EP -> CPU fallback
os.makedirs(art, exist_ok=True)
mp = "/home/root/models/public/resnet18_opset9_4batch.onnx"   # input [4,3,224,224] UINT8 (norm folded in)

opts = {
    "tidl_tools_path": os.environ["TIDL_TOOLS_PATH"],
    "artifacts_folder": art,
    "platform": "J7", "tensor_bits": 8, "accuracy_level": 1, "debug_level": 1,
    "advanced_options:calibration_frames": 4, "advanced_options:calibration_iterations": 4,
}
so = rt.SessionOptions(); so.log_severity_level = 1
sess = rt.InferenceSession(mp, providers=["TIDLCompilationProvider", "CPUExecutionProvider"],
                           provider_options=[opts, {}], sess_options=so)
assert "TIDLExecutionProvider" in sess.get_providers(), "TIDL EP fell back to CPU!"
inp = sess.get_inputs()[0]
imgs = sorted(glob.glob("/home/root/test_data/*.jpg"))
load = lambda p: np.asarray(Image.open(p).convert("RGB").resize((224, 224))).transpose(2, 0, 1)
for f in range(4):
    batch = np.stack([load(imgs[(f*4+k) % len(imgs)]) for k in range(4)]).astype(np.uint8)
    sess.run(None, {inp.name: batch})         # calibration frames
del sess                                       # finalize artifacts
print("ARTIFACTS:", sorted(os.listdir(art)))
```
Run it with `SOC=am68pa`, `TIDL_TOOLS_PATH=/home/root/tidl_tools`,
`LD_LIBRARY_PATH=/home/root/tidl_tools:/home/root/tidl_tools/osrt_deps`. Success =
`Subgraph Compiled Successfully` and a **C7x: 52 / CPU: 0** node split. Outputs:
`subgraph_0_tidl_net.bin` (the network) + `subgraph_0_tidl_io_1.bin` (IO config) +
`allowedNode.txt` + `onnxrtMetaData.txt`. Copy these to the board's model-artifacts folder.

### Compile gotchas

| Symptom | Cause / fix |
|---|---|
| `status == true was false` EP error | `onnxrt_ep.py` am69a opts → use the minimal options dict above |
| `artifacts_folder is not empty` | `shutil.rmtree` the folder before each compile |
| `expected uint8, got float` | feed raw **uint8** calib images (normalization is folded into the graph) |
| assert says EP failed but it didn't | `get_providers()` reports `TIDLExecutionProvider`, not `…Compilation…` |
| model download = 2 bytes | `git.ti.com` 503s → use a model hosted on `software-dl.ti.com` |
| pip read-timeout / torch corrupt in container | `ENV PIP_DEFAULT_TIMEOUT=120 PIP_RETRIES=10`; install CPU torch from `download.pytorch.org/whl/cpu` |

---

## 4. Running inference on the board (the resnet proof)

With the fleet up (Part 2), the runtime installed (Part 1), and the artifact in place (Part 3):

```bash
source ~/tidl/setenv.sh                 # the ~/tidl venv with TIDLExecutionProvider
export SOC=am68pa
cd <edgeai-tidl-tools>/examples/osrt_python/ort
python3 onnxrt_ep.py -m cl-ort-resnet18-v1_4batch
```
Expected: `libtidl_onnxrt_EP loaded`, `Final number of subgraphs created are : 1, Offloaded Nodes
52, Total Nodes 52`, a real top-1 class (e.g. `warplane` on the airshow test image),
`IPC: Init ... Done` with **no** "Unable to create TX channels" errors, exit 0. Lingering
`_rpmsg_char_find_*` warnings during teardown are benign.

---

## 5. Debugging toolbox

- **Per-core boot log (best tool, no serial cable).** Every core writes `appLogPrintf` output
  into a slot of the non-cacheable `rtos-log@0xAC000000` carveout, so the A72 can read it through
  `/dev/mem` with no cache maintenance. Slot = `0xAC000000 + cpu_idx*0x4000`; cpu_idx:
  `mpu1_0=0, DM=1, mcu2_0=3, mcu2_1=4, c6x_1=7, c6x_2=8, c7x_1=9`. Per-slot layout:
  `+0x00 rd_idx, +0x04 wr_idx, +0x08 valid(==0x01357231), +0x0C sync_state, +0x10 name[16],
  +0x20 text`. Read it with `mmap("/dev/mem", O_SYNC, offset=0xAC000000)` in Python (`dd` gets
  EFAULT; `mmap` works).
- **Firmware load map.** `readelf -l <fw>.out | grep LOAD` → the PT_LOAD `PhysAddr` ranges drive
  carveout sizing; `readelf -S | grep -i 'resource\|tracebuf\|vring'` for IPC regions.
- **Topology.** `/sys/class/remoteproc/*/{state,firmware}` + `readlink device`. The R5F
  firmware-name is *derived* (`j7-main-r5fX_Y-fw` for `main_r5fssX_coreY`) — there is no
  `firmware-name` DT property.

### Benign messages (not failures)
- `dmesg`: `unsupported resource 65538` — a TI vendor resource the stock Debian remoteproc skips.
- No `remoteproc*/trace0` for the vision cores — they log to `rtos-log@ac000000`, not the
  mainline trace resource.

---

## 6. Known rough edges

- **mcu2_0 DSS-init abort (cosmetic).** With the fleet synced, mcu2_0 runs past the barrier and
  aborts in the display (DSS) pipeline (`DFAR=0xa010000`) — a display DMA into an unmapped region.
  It does **not** affect TIDL inference (the graph is C7x-only and mcu2_0's rpmsg endpoint
  registers before DSS). Fix by building mcu2_0 without DSS (this platform drives no display).
- **Reversibility.** Everything here is firmware + device-tree only — the Debian kernel/userland
  is untouched. The stock `.tisdk` firmware symlinks and the recovery boot label (stock DTB) let
  you fall back at any point.
