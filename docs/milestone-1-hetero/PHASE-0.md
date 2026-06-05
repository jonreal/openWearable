# Phase 0 — Board ground-truth & toolchains

> **Objective:** establish the *real* state of the board and install/verify every
> toolchain, so Phases 1+ are built on facts rather than guesses. Phase 0 writes almost
> no project code — its deliverable is a filled-in **`BOARD-TRUTH.md`** plus a working
> set of compilers and reference SDK files.
>
> **Where this runs:** mostly **on the board over ssh**. A few steps are on the laptop.
>
> **Definition of done:** §"Phase 0 exit checklist" at the bottom is fully checked, and
> `docs/milestone-1-hetero/BOARD-TRUTH.md` exists with every `<FILL IN>` replaced by a
> real value.

Read `00-OVERVIEW.md` first. Do **not** guess board specifics — capture them.

---

## Prerequisites
- ssh access to the BeagleBone AI-64 (sources at `/root/openWearable`, branch `am64x`).
- The existing `apps/template` builds and runs today (this is our regression anchor).

---

## Step 0.1 — Confirm board & SoC identity
On the board:
```bash
cat /proc/device-tree/model            # expect a BeagleBone AI-64 string
uname -a                               # kernel version (record it)
cat /etc/dogtag 2>/dev/null            # BeagleBoard image tag, if present
ls /proc/device-tree/                  # sanity: this is a k3/j721e tree
```
**Record in BOARD-TRUTH.md:** model string, kernel version, image tag.

> Sanity note: this confirms TDA4VM/J721E (A72 cores) — the `cortex-a72` compiler flag and
> the `j721e` build naming are in place; only the `am64x` branch rename is deferred.

---

## Step 0.2 — Inventory remoteproc nodes (THE critical step)
The generalized loader (Phase 2) is data-driven off this inventory. Map every remoteproc
instance to a physical core, its current state, and its firmware.

```bash
# List all remoteproc instances and their human names + state
for d in /sys/class/remoteproc/remoteproc*; do
  printf '%s\t%s\t%s\n' "$d" "$(cat $d/name 2>/dev/null)" "$(cat $d/state 2>/dev/null)"
done

# The /dev/remoteproc symlinks the current code uses (j7-pru0_0 etc.)
ls -l /dev/remoteproc/ 2>/dev/null

# Per-instance firmware currently assigned
for d in /sys/class/remoteproc/remoteproc*; do
  printf '%s -> firmware=%s state=%s\n' "$(cat $d/name)" \
    "$(cat $d/firmware 2>/dev/null)" "$(cat $d/state)"
done
```
**Record in BOARD-TRUTH.md** a table with columns:
`remoteprocN | name | physical core | /dev symlink | boot state (running/offline) | current firmware | v1 use? (R5F0/C7X0/C66X0/PRU0/PRU1/—) | available or RESERVED`.

**Identify and flag:**
- The **MCU R5F core0** node (device-manager/TIFS) → mark **RESERVED, do not touch**.
- Which nodes the kernel **auto-booted** (`state = running` at boot) — these are the
  cores we'll **take over** at runtime (stop → set our firmware → start).
- The PRU nodes that today's `pru.c` reaches via `/dev/remoteproc/j7-pru0_0` and
  `_1` — confirm which `remoteprocN` they are.

---

## Step 0.3 — Pick v1 core instances
From 0.2, choose the concrete instances for the v1 set and record them:
- `R5F0` = first **available** main-domain R5F (NOT MCU R5F core0).
- `C7X0` = the C7x node.
- `C66X0` = first C66x node.
- `PRU0`, `PRU1` = the two PRU nodes already in use.

**Record the exact `name` strings and sysfs paths** — Phase 2's descriptor table uses
them verbatim.

---

## Step 0.4 — Install / verify toolchains
Goal: one working compiler per core type, versions recorded.

Already present (verify): `clpru -version`, `gcc --version`.

Install/verify the rest (TI Code Generation Tools — from the TI website "Arm/C6000/C7000
CGT" downloads, or as bundled by the J721E Processor SDK / MCU+ SDK):
```bash
cl7x --version          # C7x      (package ti-cgt-c7000)
cl6x --version          # C66x     (package ti-cgt-c6000)
tiarmclang --version    # R5F      (package ti-cgt-armllvm)   [or: arm-none-eabi-gcc --version]
```
**Record in BOARD-TRUTH.md:** for each tool — found? version, absolute install path
(e.g. `/usr/share/ti/cgt-*` or `/opt/ti/...`). These paths feed the Makefile in Phase 3
(compare with the existing `CGT_DIR = /usr/share/ti/cgt-pru` convention).

> If a compiler is missing, installing it is part of Phase 0. Prefer matching the
> versions used by the J721E SDK already on the board to keep resource-table/ABI
> compatibility.

---

## Step 0.5 — Locate TI reference resource tables + startup
Each remoteproc core needs a valid `.resource_table` section and minimal startup/vectors
in its ELF. We will adapt TI's, not hand-roll (except PRU, which already has
`library/include/rsc_table_pru0.h` / `_pru1.h`).

On the board (paths vary by SDK install — search):
```bash
# Find installed SDK roots
ls -d /opt/ti/* /usr/share/ti/* 2>/dev/null
# Hunt for reference resource tables and IPC echo/empty examples per core
find /opt/ti /usr/share/ti -iname '*rsc_table*'      2>/dev/null
find /opt/ti /usr/share/ti -ipath '*ipc*echo*'        2>/dev/null
find /opt/ti /usr/share/ti -ipath '*r5f*linker*' -o -ipath '*c7x*linker*' -o -ipath '*c66*linker*' 2>/dev/null
```
**Record in BOARD-TRUTH.md:** absolute paths to a usable resource table + linker command
file + startup for **R5F**, **C7x**, and **C66x**. If the SDK isn't installed, note that
installing the J721E MCU+ SDK (or PROCESSOR-SDK-RTOS) is a Phase-0 task.

---

## Step 0.6 — Inventory existing memory carveouts & find free space
The unified memory model (Phase 1) needs a physical region that (a) every core can reach
and (b) doesn't collide with existing carveouts.

```bash
# Reserved-memory regions Linux knows about
ls /proc/device-tree/reserved-memory/ 2>/dev/null
for n in /proc/device-tree/reserved-memory/*/; do
  echo "== $n"; ls "$n"; done
# Hex-dump the 'reg' of each to get base/size (or read the .dts directly, below)
cat /sys/kernel/debug/memblock/reserved 2>/dev/null

# DDR layout
cat /proc/iomem | grep -iE 'system ram|reserved'
```
Also read the carveouts straight from the source DTS on the laptop:
`device-tree/k3-j721e-boneai64-openWearable.dts` (and the upstream `.dtsi` files it
includes) — the default tree already reserves per-core IPC + DDR regions for the
R5F/C7x/C66x.

**Record in BOARD-TRUTH.md:**
- Every existing reserved-memory `reg` (base + size) — the "occupied" map.
- Candidate **free** physical ranges for our carveout (DDR base on J721E is
  `0x0_8000_0000`; existing carveouts often sit in the high DDR region — pick a gap).
- **MSMC question:** is MSMC SRAM available as carveable SRAM, or is the 8 MB configured
  as L3 cache? If carveable, prefer it for the control plane (lower latency); else plan a
  small DDR `no-map` carveout for the control plane. Record the decision + addresses.

---

## Step 0.7 — Capture the exact device-tree build & deploy procedure
The maintainer already has a working "recompile the `.dts`, then reboot" flow for this
board (the README's AM335x `make install_arm` path does **not** apply to the k3/J721E
arm64 tree). Capture it **literally** so a fresh instance can reproduce it.

**Record in BOARD-TRUTH.md, verbatim:**
- Source `.dts` in use: `device-tree/k3-j721e-boneai64-openWearable.dts` (confirm it's the
  active one — cross-check with the running model / `extlinux.conf`).
- The exact commands to compile it to a `.dtb` (kernel `dtc`, or BeagleBoard-DeviceTrees
  k3 path, etc.).
- Where the `.dtb` is installed (`/boot/firmware/...`) and how boot selects it
  (`extlinux.conf` / `uEnv.txt` — see `device-tree/extlinux.conf.sample`,
  `device-tree/uEnv.txt.sample`).
- The reboot step.

---

## Step 0.8 — Regression anchor: build & run today's `template`
Before changing anything, prove the current 2-PRU path works on this board, so later
regressions are unambiguous.
```bash
cd /root/openWearable/library && make
cd /root/openWearable/apps/template && make
/root/openWearable/apps/bin/template        # exercise the menu; confirm PRUs run
```
**Record in BOARD-TRUTH.md:** confirmation it built + ran, and the observed PRU behavior
(e.g., debug-pin toggling, `pru0var`/`pru1var` advancing in logs). This is the baseline
Phase 2 must reproduce through the *new* generic loader.

---

## Phase 0 exit checklist
- [ ] Board model, SoC, kernel version recorded.
- [ ] Full remoteproc inventory table filled in; MCU R5F0 flagged RESERVED; auto-booted
      cores identified.
- [ ] v1 core instances (R5F0/C7X0/C66X0/PRU0/PRU1) pinned to exact `name`/sysfs paths.
- [ ] `clpru`, `gcc`, `cl7x`, `cl6x`, `tiarmclang` all present; versions + paths recorded.
- [ ] Reference resource table + linker cmd + startup located for R5F, C7x, C66x.
- [ ] Existing carveouts mapped; free physical range chosen; MSMC-vs-DDR control-plane
      decision made with concrete addresses.
- [ ] Exact `.dts → .dtb →` reboot procedure captured verbatim.
- [ ] Today's `template` app builds and runs (regression baseline recorded).
- [ ] `docs/milestone-1-hetero/BOARD-TRUTH.md` committed with no `<FILL IN>` left.

**Hand-off note:** Phase 1's memory map plugs the addresses chosen in 0.6 into
`memmap.h`. Do not start Phase 1's device-tree carveout until 0.6 and 0.7 are done.
