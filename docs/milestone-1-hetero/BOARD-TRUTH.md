# BOARD-TRUTH — BeagleBone AI-64 (live findings)

Filled in from a live session on **2026-06-04** over ssh (`root@boneai64-0.local`,
192.168.68.73). This is the factual backing for the Phase 1 memory map and Phase 2/3
loader. Status legend: ✅ confirmed on board · ⚠️ decision/needed · ⬜ not yet captured.

## 0.1 Board identity ✅
- Model: `BeagleBoard.org BeagleBone AI-64 openWearable-v0` (our device tree is live).
- Kernel: `6.12.17-ti-arm64-r33`, Debian 12 (Bookworm Minimal, 2024-09-04 image).
- SoC: TI **TDA4VM / J721E**. App cores = Cortex-A72.
- DDR: `MemTotal ≈ 3.82 GB` (4 GB minus carveouts).
- ssh: **key auth already works** (no password needed from this laptop).

## 0.8 Regression anchor ✅
`apps/template` builds and runs. `template -v` shows PRUs initialized, shared memory
mapped (Param 48 B / LUT 2000 B / Data 1548 B), and the RT state ring advancing
deterministically. **This is the baseline the new generic loader must reproduce.**

---

## 0.2 / 0.3 Remoteproc inventory & v1 core mapping ✅

All cores are enumerated under `/sys/class/remoteproc/`. **Key finding: every core we want
is `offline` (NOT auto-booted) except the reserved MCU R5F — so there is nothing to "take
over"; we just write firmware + start, exactly like the PRUs today.**

### v1 core set → concrete instances (LOCKED)
| v1 name | remoteproc | device         | /dev symlink        | boot state | default fw            |
|---------|-----------|----------------|---------------------|------------|------------------------|
| R5F0    | remoteproc1 | `5c00000.r5f`  | `j7-main-r5f0_0`    | offline    | `j7-main-r5f0_0-fw`    |
| C7X0    | remoteproc5 | `64800000.dsp` | `j7-c71_0`          | offline    | `j7-c71_0-fw`          |
| C66X0   | remoteproc3 | `4d80800000.dsp`| `j7-c66_0`         | offline    | `j7-c66_0-fw`          |
| PRU0    | remoteproc6 | `b034000.pru`  | `j7-pru0_0`         | offline    | `am64x-pru0-template-fw` |
| PRU1    | remoteproc9 | `b038000.pru`  | `j7-pru0_1`         | offline    | `am64x-pru1-template-fw` |

### Reserved — DO NOT TOUCH
- **remoteproc0 `41000000.r5f`** — state **`attached`** (the only running core). This is the
  **MCU R5F running the Device Manager / TIFS**. Never repurpose.

### Other available cores (deferred past v1, but present)
- 2nd main R5F: **remoteproc2 `5e00000.r5f`** (`j7-main-r5f1_0`), offline.
  - Note: only 2 *main* R5F remoteproc nodes are exposed (`5c00000`, `5e00000`) — the
    clusters are in **lockstep mode** (each shows its core0). Getting 4 independent main
    R5F cores would require a DT change to **split mode**. (6 `r5f-memory` carveouts exist
    at `a0…a5` for all possible R5Fs.)
- 2nd C66x: **remoteproc4 `4d81800000.dsp`** (`j7-c66_1`), offline.
- A large ICSSG fleet beyond PRU0/1: ICSSG0 `rtu0_0/0_1`, `txpru0_0/0_1`; ICSSG1
  `pru1_0/1_1` (`b134000`/`b138000`), `rtu1_*`, `txpru1_*` — all offline.

> The current `template` uses `j7-pru0_0` (remoteproc6) + `j7-pru0_1` (remoteproc9) =
> ICSSG0's two PRUs. Their firmware fields still read `am64x-…-fw` → naming wart, Phase 5.

---

## 0.4 / 0.5 Toolchains & SDK ⚠️ (GATING)

| Tool       | Core | Status | Path |
|------------|------|--------|------|
| `clpru`    | PRU  | ✅ present | `/usr/bin/clpru` |
| `gcc`      | A72  | ✅ present | `/usr/bin/gcc` (⚠️ Makefile must target `cortex-a72`) |
| `cl7x`     | C7x  | ❌ **MISSING** | — |
| `cl6x`     | C66x | ❌ **MISSING** | — |
| `tiarmclang` | R5F | ❌ **MISSING** | — |
| `arm-none-eabi-gcc` | R5F (alt) | ❌ MISSING | — |

- Only TI SDK component installed: **`/usr/share/ti/cgt-pru`**. No MCU+ SDK / PSDK-RTOS,
  so **no reference resource tables / linker cmd / startup for R5F / C7x / C66x yet** (0.5
  is unmet).

**⚠️ This is the critical-path prerequisite for Phase 3 — today the board can only build
PRU + A72 code.**

- ✅ **Build host DECIDED: native on the board, in C — no cross-compiling, no C++.** All
  firmware (PRU/R5F/C7x/C66x) is built on the AI-64 itself, the same native pattern as the
  already-installed `clpru` (`/usr/bin/clpru`, via `apt ti-pru-cgt-v2.3`).
- ⬜ **Remaining task:** install the **native arm64** TI CGTs `cl7x` (C7000), `cl6x`
  (C6000), `tiarmclang` (Arm), plus a J721E SDK (MCU+ SDK is the lightest) for the
  R5F/C7x/C66x **resource tables + startup**. Find apt packages or TI native-arm64
  installers; record exact names in `../SETUP.md` §B.1 once resolved.

---

## 0.6 Memory map ✅ (carveouts) + ⚠️ (our region proposal)

### Existing reserved-memory (all in LOW DDR, 32-bit reachable)
| Region | Base | Size |
|--------|------|------|
| optee  | `0x9e800000` | `0x1800000` (24 MB) |
| r5f ×6 (dma 1 MB + mem 15 MB each) | `0xa0000000`–`0xa6000000` | 16 MB × 6 |
| c66 ×2 (dma 1 MB + mem 15 MB each) | `0xa6000000`–`0xa8000000` | 16 MB × 2 |
| c71 (dma 1 MB + mem 15 MB)         | `0xa8000000`–`0xa9000000` | 16 MB |
| ipc-memories | `0xaa000000` | `0x1c00000` (28 MB) → ends `0xabc00000` |

So the SDK reserves a contiguous block **`0x9e800000`–`0xabc00000`** (with a 16 MB System
RAM hole at `0xa9000000`–`0xaa000000`).

### System RAM (free for carving via `no-map`)
`0x80000000`–`0x9e7fffff` · `0xa9000000`–`0xa9ffffff` · **`0xabc00000`–`0xffffffff`** (≈1.07 GB) ·
`0x880000000`–`0x8ffffffff` (high 2 GB).

> ⚠️ **R5F and PRU are 32-bit masters** — they cannot reach the high DDR region
> (`0x8_8000_0000`) without RAT translation. Our shared planes **must live in low DDR**
> (`< 0x1_0000_0000`), in the same neighborhood as the SDK's core carveouts. This is why
> the SDK keeps all remoteproc memory below `0xac000000`.

### MSMC / on-chip SRAM (future low-latency option)
- `0x70000000`–`0x707effff` = **MSMC SRAM (~8 MB)**, currently claimed as `70000000.sram`
  (mmio-sram node). Carving a partition here for the control plane is a **later
  optimization** (lower latency than DDR); needs an `sram` child partition node. Not for v1.
- `0x41c00000`–`0x41cfffff` = 1 MB MCU-domain SRAM (MCU use; leave it).

### PROPOSED openWearable carveout ⚠️ (verify no collision, then add to DT in Phase 1.4)
Place both planes in the free low-DDR block right after `ipc-memories`, 32-bit reachable:
| Node      | Base | Size | Plane / cache |
|-----------|------|------|----------------|
| `ow_ctrl` | `0xac000000` | `0x00100000` (1 MB) | control — **non-cacheable** |
| `ow_data` | `0xac100000` | `0x04000000` (64 MB) | data — **cacheable** |

(Ends at `0xb0100000`; well inside the `0xabc00000–0xffffffff` System RAM block. Adjust
`ow_data` size later as NN weights/history needs firm up. These become the
`OW_CTRL_PHYS_BASE` / `OW_DATA_PHYS_BASE` in `memmap.h`.)

---

## 0.7 Device-tree build & deploy procedure ✅ (verified; full steps in `../SETUP.md` §C.2)
Active DTS: `device-tree/k3-j721e-boneai64-openWearable.dts` (live; model string confirms).
Build tree: **`/root/BeagleBoard-DeviceTrees`** (present), `dtc` at `/usr/bin/dtc`.
1. Symlink the dts into `~/BeagleBoard-DeviceTrees/src/arm64/ti/`.
2. `cd ~/BeagleBoard-DeviceTrees && make install_arm64` → compiles `src/arm64/ti/*.dts`,
   copies dtbs to **`/boot/firmware/ti/`** (confirmed: `k3-j721e-boneai64-openWearable.dtb`
   is there).
3. Boot selects it via **`/boot/firmware/extlinux/extlinux.conf`** — active "eMMC (default)"
   label has `fdt /ti/k3-j721e-boneai64-openWearable.dtb`. ✅
4. `reboot`, then verify `cat /proc/device-tree/model`.

→ The Phase 1.4 `ow_ctrl`/`ow_data` carveout will be added to this dts and deployed by
this exact procedure.

---

## Phase 0 status summary
- ✅ Done: 0.1 identity, 0.2/0.3 remoteproc map + v1 instances, 0.6 memory map, 0.7 dtb
  build/deploy procedure, 0.8 regression.
- ⬜ **Only remaining task (GATES Phase 3):** install native-arm64 `cl7x`/`cl6x`/`tiarmclang`
  + a J721E SDK (resource tables/startup). Build host is decided (see below).

### Decisions resolved by these findings
- **Build host → native on the board, C only, no cross-compiling.**
- **Take-over vs coexist → resolved:** target cores are `offline`; no take-over needed.
- **Control-plane location → DDR `no-map` for v1** (`0xac000000`), MSMC deferred.
- **v1 R5F instance → `5c00000.r5f` (remoteproc1)**; lockstep mode (split-mode = future DT).
- **DTB deploy → `make install_arm64` in `/root/BeagleBoard-DeviceTrees` → `/boot/firmware/ti/`
  → `extlinux.conf`.**
