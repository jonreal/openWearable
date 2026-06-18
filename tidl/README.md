# tidl — C7x + MMA TIDL inference firmware (J721E / TDA4VM, BeagleBone AI-64)

A **fleet-free** firmware for the **C7x DSP + MMA** that runs TIDL neural-network inference with
**no vision_apps fleet** (no TIOVX graph, no IPC framework, no cpu-sync barrier). The A72 (Linux)
loads it via remoteproc, pushes input features + a compiled net into a shared-memory mailbox, and
reads the output tensor back. This is openWearable's on-device ML inference path — the home of the
EMG transformer to come.

> **Why its own top-level dir (not `library/`)?** The C7x is the **only** core whose firmware
> cannot be built on the board: TI's C7000 codegen (`cl7x`) is x86-only, and the build needs the
> multi-GB TI PSDK. So this firmware is **built off-board on an x86 Linux host and committed as a
> prebuilt binary**; the board just symlinks + loads it. (The on-board-buildable cores — PRU now,
> C66/R5F later — live in `library/`.) Everything openWearable *can* own lives here; only TI's SDK
> + compilers stay external, pinned in [`third_party/ti/MANIFEST.md`](../third_party/ti/MANIFEST.md).

## Layout
```
tidl/
  README.md            ← you are here (the full runbook)
  Makefile             the WHOLE build — cl7x only, no fleet/concerto (see "Rebuild" below)
  src/                 the 4 sources compiled into the firmware (+ provenance)
    ow_c7x_tidl.c        OW platform layer: scratch pools, 32MB arena, the A72<->C7x mailbox,
                         and ow_c7x_tidl_run() (the create-once / invoke-many service loop)
    platform_defines.h   OW build config (L4_MEM_SIZE must match s_l4[] in ow_c7x_tidl.c)
    main.c               vendored c7x boot (OW: appMain runs ow_c7x_tidl_run; mailbox non-cacheable)
    tidl_rt.c            vendored TIDL device runtime (OW: the scratch-corruption fix)
    PROVENANCE.md        origin + change for each file
  linker/              the 2 linker scripts the Makefile uses + j721e_c7x_lnk.cmd (Path-X, future)
  firmware/            the prebuilt, committed deploy artifact (vx_app_rtos_linux_c7x_1.out)
  host/                A72 board-side tools (run on the board, pure stdlib — no numpy)
    ow_c7x_infer.py      stage net+input, trigger, read output, compare to ref
    ow_dru_dump.py       read the C7x DRU registers via /dev/mem (debug); `watch` mode
  models/              the hello-world MLP + how its TIDL artifacts are compiled
```

## The two-host model
| Step | Where | Needs |
|---|---|---|
| **Rebuild firmware** | x86_64 Linux | TI PSDK-RTOS 10.01.00.04 + **only** the C7000 CGT (`cl7x` 4.1.0) |
| **Compile a model** | x86_64 Linux | edgeai-tidl-tools (Docker) — see `models/README.md` |
| **Deploy + run** | the board (A72) | nothing extra — symlink the committed ELF, reboot |

---

## Quick start — deploy the prebuilt firmware (no x86 host needed)
The repo is checked out on the board at `/root/openWearable`, and the firmware is committed, so
deploy is just a symlink + reboot. **This swaps the C7x firmware — a board state change; get the
usual go-ahead first.**
```bash
# on the board (A72):
ln -sf /root/openWearable/tidl/firmware/vx_app_rtos_linux_c7x_1.out /lib/firmware/j7-c71_0-fw
sync && reboot
# after it comes back, confirm the C7x is up:
cat /sys/class/remoteproc/remoteproc2/state     # -> running   (remoteproc2 = 64800000.dsp = C7x)
```
Then run an inference:
```bash
cd /root/openWearable
python3 tidl/host/ow_c7x_infer.py tidl/models/ow_tiny_net.bin tidl/models/ow_tiny_io.bin
# -> C7x status=0  output=[...]  float ref=[...]  max|abs err|=0.0449  PASS
```
Restore the stock resnet-capable fleet at any time: `ln -sf vx_app_rtos_linux_c7x_1.out.ow
/lib/firmware/j7-c71_0-fw` (if that backup is present) + reboot.

---

## Rebuild the firmware (x86 host) — one `make`, `cl7x` only
No fleet build, no concerto, no R5F/C66/A72 toolchains. `make` compiles our 4 sources + two
single-file SDK helpers + the exact C71/FreeRTOS `app_utils` sources, and links the SDK's
**prebuilt** libs (CSL, udma, dmautils, sciclient, osal, FreeRTOS kernel, mmalib, TIDL).

1. **Install** the TI PSDK-RTOS 10.01.00.04 + the **C7000 CGT 4.1.0** (only that compiler is
   needed; see [`third_party/ti/MANIFEST.md`](../third_party/ti/MANIFEST.md)).
2. **Build:**
   ```bash
   make -C tidl/ PSDK=/path/to/ti-processor-sdk-rtos-j721e-evm-10_01_00_04
   #   (or edit the PSDK/CGT defaults at the top of tidl/Makefile, then just: make -C tidl/)
   ```
   Output: `tidl/firmware/vx_app_rtos_linux_c7x_1.out` (stripped). Commit the updated ELF + note it
   in `firmware/PROVENANCE.md`. The Makefile encodes the exact cl7x flags + the trimmed link line
   (only the libs the firmware actually references); rerun it whenever `src/` changes.

> The optional **fleet** build (`third_party/ti/build-firmware.sh`) is a *separate* path — it stands
> up the full vision_apps fleet for the resnet/onnxruntime sanity check. Our firmware does **not**
> depend on it.

## Compile a model (x86 host)
See [`models/README.md`](models/README.md). In short: `gen_tiny_mlp.py` builds the ONNX,
`compile_tiny.py` compiles it to `subgraph_0_tidl_net.bin` + `..._io_1.bin` via the edgeai-tidl-tools
Docker container (SOC `am68pa`); rename to `ow_tiny_net.bin` / `ow_tiny_io.bin`.

---

## How it works
**Boot (Path-Y).** We reuse vision_apps' proven C7x boot (`platform/j721e/rtos/c7x_1/main.c`:
MMU/cache/CLEC/DRU-QoS init, all working on this board) and only swap the `appMain` task body —
`appInit(); appRun();` (the fleet chain) becomes `ow_c7x_tidl_run()`. `main()` still runs
`appC7xClecInitDru()` + `setup_dru_qos()` (DRU→C7x event routing), so the DMA path is fully wired.

**Runtime.** `ow_c7x_tidl_run()` (in `src/ow_c7x_tidl.c`): init once (`appSciclientInit` →
`appUdmaInit` → `gUdmaDrvObjPtr`), then a **service loop** — wait for the mailbox `READY` flag,
`TIDLRT_create` on the first kick (engine + MMA + DRU channels init; all 16 IALG memrecs allocate
from our 32 MB DDR arena), then per kick bind float IO tensors → `TIDLRT_invoke` (`algProcess`, the
MMA compute) → write the output tensor, bump the invoke counter, clear `READY`, set `DONE`. The net
is created once and reused (load-once / run-many); a different net needs a reboot.

**A72↔C7x DDR mailbox** @ `0xB8000000`, mapped **non-cacheable** (MAIR4) so the A72 `/dev/mem`
(O_SYNC) sees it coherently with zero cache management:

| Offset | Field | Offset | Field |
|---|---|---|---|
| `0x00` | `READY` (A72→C7x, `0xC7C7AA01`) | `0x40` | `OW_DBG[]` progress markers (10 words) |
| `0x10` | `DONE` (C7x→A72, `0xC7C7DD01`)  | `0x200` | firmware text log ring |
| `0x20` | `STATUS` (TIDLRT return code)   | `0x100000` | `NET` (compiled net.bin) |
| `0x30` | `NETLEN` / `0x38` `IOLEN`       | `0x2000000`/`0x2100000`/`0x2180000` | `IO` / `INPUT` / `OUTPUT` |

## Debug
- **Progress + result:** `ow_c7x_infer.py` prints the `OW_DBG` markers (`enter`, `create_st`,
  `pre_invoke`, `invoke_st`, …) and the firmware text log — so a hang tells you exactly how far it got.
- **DMA state:** `python3 tidl/host/ow_dru_dump.py [label]` dumps the C7x DRU registers from the A72
  via `/dev/mem` (global CFG, queues, per-channel CHRT/CHNRT/CHATOMIC, CAUSE). `… watch` polls for
  enabled channels mid-transfer. This is how the `algProcess` stall was isolated — see
  `notes/milestone-1-hetero/C7X-TIDL-RT-RECON.md`.

## The bug that took ~14 board cycles (now fixed)
`algProcess` used to hang. Root cause: TI's `tidl_rt.c` is a **test harness** whose `pFxnUnLock`
callback `memset`s `0xDA` over the L1/L2/L3 scratch on every engine critical-section exit
("code to corrupt buffers"), wiping the live MMA/DRU stream scratch mid-inference. The fix lives in
the vendored `src/tidl_rt.c` (real `HwiP_disable/restore`, memset removed — mirrors the production
`vx_tidl_target.c`); see `src/PROVENANCE.md`. Full forensics (register diff vs the stock fleet,
source diff) in the recon note.

## Status & limitations
- ✅ Fleet-free C7x TIDL inference verified on hardware, reproducible across reboots.
- ✅ **Multi-invoke**: the service loop runs back-to-back inferences without a reboot (the host tool
  prints `invoke#`).
- ⚠️ `max|abs err| 0.0449` is int8 quantization (ReLU zeros match exactly) — tighten with better
  calibration when accuracy matters.
- `linker/j721e_c7x_lnk.cmd` is for the **future Path-X lean firmware** (hand-written boot, no
  FreeRTOS/vision_apps); Path-Y (this firmware) does not use it.
