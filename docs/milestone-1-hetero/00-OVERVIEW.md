# Milestone 1 — Heterogeneous `template-hetero` app — OVERVIEW

> **Read this first.** It captures the *locked* architecture decisions and the rationale
> behind them. The Phase docs (`PHASE-0.md`, `PHASE-1.md`, …) are the step-by-step
> execution; this file is the "why" that makes them make sense. If you only read one
> file before touching code, read this one.

Branch: `am64x`  ·  Board: **BeagleBone AI-64**  ·  SoC: **TI TDA4VM / J721E**

---

## 1. Goal

Replace the current 2-tier system (PRU + Linux) with a **heterogeneous mesh** spanning
every compute core on the TDA4VM, and prove it with a new reusable app,
**`template-hetero`** — the heterogeneous "hello world." Once this exists, every future
app (the EMG transformer included) is "fill in the per-core `Update` hooks."

`template-hetero` must deliver three things:

1. **A unified two-plane memory model** that all cores map and agree on.
2. **A generalized ARM entry layer** that loads, starts, and health-checks *every* core
   via remoteproc (the multi-core generalization of today's `PruInit`/`main.c`).
3. **One minimal firmware per core type**, each running an `Init/Update/Cleanup`
   heartbeat (the heterogeneous analog of today's `pru0var++`), all handshaking through
   the shared model **in unison**.

No neural network yet. This is the scaffold the NN drops into later.

---

## 2. Core inventory & v1 role mapping (LOCKED)

The TDA4VM has more cores than v1 will use. **v1 brings up exactly one of each type:**

| Core (v1)   | Type / clock              | v1 role in `template-hetero`            | Notes |
|-------------|---------------------------|------------------------------------------|-------|
| A72         | 2× Cortex-A72, Linux      | Orchestration: load all cores, map shared mem, UI, logging | `main.c` |
| **R5F0**    | Cortex-R5F @ 1 GHz, FPU   | Hard real-time control brain (heartbeat in v1) | One of the 5 available R5Fs |
| **C7X0**    | C7x DSP + MMA (~8 TOPS)   | Async ML-inference engine (heartbeat in v1) | Future home of the transformer |
| **C66X0**   | C66x DSP                  | Async classical-DSP front-end (heartbeat in v1) | Future EMG filtering/features |
| **PRU0**    | PRU-ICSSG @ 200 MHz       | Lowest-level deterministic I/O + timer tick | As today |
| **PRU1**    | PRU-ICSSG @ 200 MHz       | I/O partner (lockstep with PRU0)         | As today |

**Off-limits:** **MCU R5F core0** runs the device-manager / TIFS (sciserver) firmware
that the system depends on — never repurpose it.

Cores deferred past v1 (model supports them, we just don't instantiate them yet): the
remaining R5Fs, the 2nd C66x, the GPU, and the video/vision accelerators.

---

## 3. LOCKED architecture decision A — Memory: a two-plane model

Cache coherence is the central new problem. On the TDA4VM the A72 cluster is internally
coherent, but the **R5F, C7x, and C66x are non-coherent masters** — each has its own
caches and nothing automatically reconciles them with each other or with the A72.

**Why the current design never hit this:** today everything shared is *uncached* on both
ends — the **PRU has no data cache**, and the A72 maps the shared SRAM via
`/dev/mem | O_SYNC` which forces a **non-cacheable** mapping. So the current model is
"coherence by avoidance," and `volatile` + the done-bit handshake is all that's needed.

That breaks once cached cores (R5F/C7x/C66x) share memory: they'll cache lines and read
stale data. But making *everything* uncached is also wrong — running a transformer
matmul out of non-cacheable memory would be brutally slow.

### Decision: split shared memory into two planes.

| Plane         | Contents                                                      | Mapping        | Coherence handled by                        |
|---------------|--------------------------------------------------------------|----------------|---------------------------------------------|
| **Control**   | handshake flags, `core_ctl[]`, the `state_t` ring, params, LUT, seqlock descriptors, intent vectors | **non-cacheable** (MSMC SRAM preferred; DDR `no-map` fallback) | *avoidance* — `volatile` just works, exactly like today |
| **Data**      | NN weights, EMG history windows, log buffers                 | **cacheable** (DDR) | *explicit* writeback (producer) / invalidate (consumer) at hand-off |

The control plane preserves today's simple programming model verbatim. The data plane
buys cache performance for throughput work at the cost of disciplined, *infrequent*
cache maintenance at producer→consumer boundaries.

Each core marks the control region non-cacheable in its own way: A72 via `O_SYNC`
(already done); R5F via an MPU region attribute; C7x/C66x via their MAR/MMU
non-cacheable setting; PRU is inherently uncached.

**Device-tree consequence:** the shared region must be a `reserved-memory` node with
`no-map` so Linux never holds a *cached* alias of it (a cached kernel alias fighting an
uncached user mapping is a classic phantom-coherence bug).

---

## 4. LOCKED architecture decision B — Synchronization: RT island + async satellites

The current model has **one timer, one frequency, one global frame**: both PRUs advance
in lockstep through the done-bit barrier. **This cannot survive hetero compute** — you
cannot put a C7x doing a multi-millisecond inference into a 1 kHz barrier without either
stalling the control loop or dragging it down to inference rate.

### Decision: replace the single synchronized frame with a small hard-real-time island
surrounded by asynchronous satellites.

```
        ┌─────────────── A72 / Linux (orchestration, UI, logging) ──── soft
        │
   ┌────┴─────  RT ISLAND  ── timer-driven at fs (>1 kHz), done-bit barrier
   │   PRU  ⇄  R5F          (the ONLY domain with a hard deadline)
   └────┬─────
        │   latest-value, lock-free handoff (fast core NEVER blocks on slow core)
   ┌────┴─────  ASYNC SATELLITES  ── free-running, each at its own rate
   │   C7x (inference ~100–500 Hz)     C66x (DSP front-end, streaming)
   └──────────
```

- **Inside the RT island (PRU + R5F):** today's done-bit barrier survives, generalized.
  This is the only place anything locksteps to `fs`.
- **Across domains (island ⇄ satellites):** switch from barriers to **latest-value,
  lock-free exchange.** The cardinal rule: *the fast core never waits on the slow core.*
  Producers publish "best available" data; bounded staleness (a few ms) is
  physiologically irrelevant for EMG (electromechanical delay alone is ~30–100 ms).

### Lock-free primitives (safe across non-coherent cores on the non-cacheable plane)
- **Seqlock** (sequence counter): writer increments a counter to *odd* before writing and
  to *even* after; reader reads counter → reads payload → re-reads counter, retrying if it
  changed or is odd. Single-writer, reader never blocks. Ideal for small payloads
  (e.g., an intent vector).
- **Ping-pong / triple buffer** with an atomic "current index": writer fills an off-buffer
  then flips the index; reader always reads the indexed buffer. No tearing; good for larger
  payloads (feature windows, inference outputs).
- **Hardware Spinlock** (J721E has a 256-lock Spinlock IP) only where genuine mutual
  exclusion is needed — avoidable entirely for single-producer/single-consumer.

**Satellite triggering (v1):** **free-running** (each satellite loops, always processing
the newest input). Mailbox-doorbell (wake-compute-sleep) is a later efficiency refinement.

**Synergy:** because the control plane is non-cacheable, the lock-free handoff needs *no*
cache-maintenance gymnastics — a seqlock on uncached MSMC is about as simple as
concurrency gets.

---

## 5. Other locked decisions

- **v1 core set:** 1× R5F, 1× C7x, 1× C66x, 2× PRU (+ A72). (§2)
- **Take-over, not reflash:** we load our bare-metal firmware **at runtime** through the
  remoteproc sysfs interface (write firmware name → start). No permanent image reflash.
  Device-tree edits (carveouts) are applied by recompiling the `.dts` and rebooting.
  **✅ Phase 0 finding:** all target cores (R5F0, C7X0, C66X0, PRU0/1) are already
  `offline` — the kernel does *not* auto-boot them — so there is nothing to "take over";
  only the reserved MCU R5F0 is running. See `BOARD-TRUTH.md`.
- **Build model:** **native on-board builds, C only — no cross-compiling, no C++.** Every
  core's firmware is compiled on the AI-64 itself (same native pattern as `clpru`).
- **Toolchains (one compiler per core type):**
  - PRU → `clpru` (`ti-cgt-pru`) — already present.
  - A72 → `gcc` (aarch64) — already present; **must target `cortex-a72`** (fix
    `-mtune=cortex-a53`).
  - R5F → **`tiarmclang`** (`ti-cgt-armllvm`) recommended; `arm-none-eabi-gcc` viable.
  - C7x → **`cl7x`** (`ti-cgt-c7000`).
  - C66x → **`cl6x`** (`ti-cgt-c6000`).
  - Bare compilers aren't enough: each remoteproc core needs a valid **resource table** +
    minimal startup/vectors in its ELF. Start from TI **MCU+ SDK / PROCESSOR-SDK-RTOS for
    J721E** IPC "empty/echo" examples and strip them down. (`rsc_table_pru0.h` is the PRU
    precedent.)

---

## 6. Milestone-1 acceptance test (definition of done)

`template-hetero` is **done** when, on the board:

1. The A72 app, via a single generalized loader, **stops → loads → starts** all six cores
   (R5F0, C7X0, C66X0, PRU0, PRU1; A72 is the host) using a data-driven core descriptor
   table.
2. Each core reports **alive** via its heartbeat field in the shared self-describing
   header, and **increments its own data substruct** in the shared `state_t` — visible in
   the A72-side log.
3. The **RT island (PRU+R5F) holds its >kHz frame** (verified by scope on PRU debug pins
   and/or R5F loop-jitter measurement) **while the async satellites (C7x, C66x) run and
   publish** via seqlock/ping-pong — and crucially, **the satellites never stall the RT
   path.**
4. Clean teardown: global disable → cores acknowledge → all stopped/restarted, no hang.

---

## 7. Phase roadmap

| Phase | Title                              | File             | Status |
|-------|------------------------------------|------------------|--------|
| 0     | Board ground-truth & toolchains    | `PHASE-0.md` → `BOARD-TRUTH.md` | **in progress** — inventory/memory/regression ✅; toolchain install + dtb-procedure capture still open |
| 1     | Unified two-plane memory model     | `PHASE-1.md`     | planned (addresses now known: ctrl `0xac000000`, data `0xac100000`) |
| 2     | Generalized ARM remoteproc loader  | `PHASE-2.md` (TBD) | not yet written |
| 3     | Bring up cores one type at a time (R5F → C66x → C7x) | `PHASE-3.md` (TBD) | not yet written |
| 4     | Unison handshake (the acceptance test) | `PHASE-4.md` (TBD) | not yet written |
| 5     | Cleanup, naming fixes, docs        | `PHASE-5.md` (TBD) | not yet written |

Phases 0 and 1 are fully specified now. Phases 2–5 are scoped here and will be written in
detail once Phase 0 returns real board numbers (some of Phase 2+ depends on them).

---

## 8. Open questions for Phase 0 (answers in `BOARD-TRUTH.md`)

- ✅ Remoteproc node ↔ core mapping & auto-boot status — done (all targets offline).
- ✅ Control-plane location — DDR `no-map` at `0xac000000` for v1; MSMC (`0x70000000`,
  ~8 MB) deferred as a latency optimization.
- ✅ `.dts` → `.dtb` build-and-deploy procedure — captured (`make install_arm64` →
  `/boot/firmware/ti/` → `extlinux.conf`; see `BOARD-TRUTH.md` §0.7 and `../SETUP.md` §C.2).
- ✅ Build host — native on the board, **C only, no cross-compiling**.
- ⚠️ **GATING (only item left):** `cl7x` / `cl6x` / `tiarmclang` are **not installed**, and
  no J721E SDK is present (only `cgt-pru`) — so there are no R5F/C7x/C66x resource
  tables/startup yet. Must install the **native arm64** CGTs + an SDK. This blocks Phase 3.
