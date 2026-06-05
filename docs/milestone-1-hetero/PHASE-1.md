# Phase 1 — Unified two-plane memory model

> **Objective:** define the single source of truth for shared memory across all cores —
> the **non-cacheable control plane** and the **cacheable data plane** (per `00-OVERVIEW.md`
> §3), the generalized handshake/state structures, the seqlock/ping-pong descriptors for
> async hand-off (§4), and the device-tree carveout that backs it. At the end of Phase 1
> the model **compiles and self-tests on the A72**; cross-core bring-up is Phases 2–4.
>
> **Prerequisite:** Phase 0 complete — you need the chosen physical addresses, the
> MSMC-vs-DDR decision, and the verified carveout-free range from `BOARD-TRUTH.md`.
>
> **Where this runs:** headers/structs authored on the laptop; device-tree carveout +
> A72 self-test verified on the board.

---

## Design principles (from the locked decisions)
1. **One source of truth for addresses.** Physical bases/sizes/offsets live in
   `library/include/memmap.h`. Every consumer — `state.h`, each core's linker `.cmd`, the
   device-tree carveout, and the ARM `mmap` — derives from it. Change them together.
2. **Two planes.** Control plane = small, non-cacheable, `volatile`-coherent. Data plane =
   large, cacheable, explicit writeback/invalidate at hand-off.
3. **Self-describing.** A header at offset 0 of the control plane carries a magic + version
   + layout so any core (and the A72 loader) can validate the model at runtime.
4. **Generalize, don't rewrite.** Extend today's `cpudata_t`/`pru_ctl_t`/`state_t` pattern
   rather than inventing a new paradigm.

---

## Step 1.1 — `memmap.h`: the address single-source-of-truth
Create `library/include/memmap.h`. Fill the `<PHASE0>` placeholders from `BOARD-TRUTH.md`.

```c
#ifndef _MEMMAP_H_
#define _MEMMAP_H_

/* ---- Physical bases (from Phase 0 BOARD-TRUTH.md) ----------------------------
 * CONTROL plane: non-cacheable. MSMC SRAM if carveable, else a DDR no-map carveout.
 * DATA    plane: cacheable DDR no-map carveout.
 * Both MUST match the device-tree reserved-memory nodes (Step 1.4) exactly.
 */
#define OW_CTRL_PHYS_BASE   0x<PHASE0>      /* e.g. an MSMC or DDR no-map base   */
#define OW_CTRL_SIZE        0x00100000      /* 1 MB control plane (ample)        */

#define OW_DATA_PHYS_BASE   0x<PHASE0>      /* DDR no-map base, != ctrl          */
#define OW_DATA_SIZE        0x04000000      /* 64 MB data plane (placeholder)    */

/* ---- Control-plane internal offsets (relative to OW_CTRL_PHYS_BASE) ----------
 * Keep page-aligned (4 KB) so per-core MMU/MPU regions can target them cleanly.
 */
#define OW_OFF_HEADER       0x00000000      /* sys_header_t                      */
#define OW_OFF_CORECTL      0x00001000      /* core_ctl_t[N_CORES]               */
#define OW_OFF_STATE        0x00002000      /* cbuff_index + state_t ring        */
#define OW_OFF_SEQLOCKS     0x0000E000      /* satellite seqlock/ping-pong descs */
#define OW_OFF_PARAM        0x0000F000      /* param_mem_t                       */
#define OW_OFF_LUT          0x00010000      /* lut_mem_t                         */

/* ---- Data-plane internal offsets (relative to OW_DATA_PHYS_BASE) -------------
 * Contents are placeholders reserved now, populated in later milestones.
 */
#define OW_OFF_WEIGHTS      0x00000000      /* NN weight blob                    */
#define OW_OFF_EMG_HIST     0x02000000      /* EMG history ring                  */
#define OW_OFF_LOGBUF       0x03000000      /* bulk log buffers                  */

#endif /* _MEMMAP_H_ */
```

> Note vs. today: the current code uses PRU shared SRAM at `global_sharedram_base
> 0x0b010000` with `param_ram_offset 0xE000` / `lut_ram_offset 0xF000`. Phase 1 *moves*
> the control plane to the Phase-0-chosen region. Keep the relative-offset *style* so the
> mental model is unchanged.

---

## Step 1.2 — Control-plane structures (rewrite `state.h` + extend `mem_types.h`)
Generalize the three existing concepts:
- `cpudata_t` (one A-core struct) → **a per-core data substruct per core type**.
- `pru_ctl_t` (one flow-control word) → **`core_ctl_t[N_CORES]`** + a global enable.
- `shared_mem_t` → **`system_mem_t`** with a self-describing header.

```c
/* state.h — control-plane shared types. EVERYTHING here is volatile and lives in the
 * NON-CACHEABLE control plane. Use 32-bit fields (packing/alignment must agree across
 * clpru / cl7x / cl6x / tiarmclang / gcc). */
#ifndef _STATE_
#define _STATE_
#include <stdint.h>

/* Stable core identity — index into core_ctl[] and the per-core data array. */
typedef enum {
  CORE_A72 = 0,
  CORE_R5F0,
  CORE_C7X0,
  CORE_C66X0,
  CORE_PRU0,
  CORE_PRU1,
  N_CORES
} core_id_t;

/* Self-describing header at OW_OFF_HEADER. */
#define OW_MAGIC   0x0E2A7AB1u      /* "openWEARABle" -ish; bump on layout change */
#define OW_VERSION 0x00010000u      /* 1.0.0 */
typedef struct {
  volatile uint32_t magic;          /* == OW_MAGIC once initialized by A72        */
  volatile uint32_t version;        /* == OW_VERSION                              */
  volatile uint32_t n_cores;        /* == N_CORES                                 */
  volatile uint32_t ctrl_size;      /* == OW_CTRL_SIZE (sanity)                   */
  volatile uint32_t off_corectl;    /* mirror of OW_OFF_* so cores can self-locate */
  volatile uint32_t off_state;
  volatile uint32_t off_param;
  volatile uint32_t off_lut;
} sys_header_t;

/* Per-core control word: generalization of pru_ctl_t.
 * enable/shdw_enable: global run gating (set by A72). done: RT-island barrier.
 * alive: monotonic heartbeat the core bumps every Update (A72 polls it). */
typedef union {
  volatile uint32_t reg;
  volatile struct {
    unsigned enable      : 1;   /* set by A72                                     */
    unsigned shdw_enable : 1;   /* shadow of enable (core-local latch)            */
    unsigned done        : 1;   /* RT-island barrier (PRU/R5F)                    */
    unsigned reserved    : 5;
    unsigned utility     : 24;  /* user-defined                                   */
  } bit;
} core_ctl_t;

/* Per-core DATA substructs (generalization of cpudata_t). v1 = a heartbeat counter
 * each; real payloads land in later milestones. Keep one struct per core TYPE. */
typedef struct { volatile uint32_t alive; volatile uint32_t var; } a72_data_t;
typedef struct { volatile uint32_t alive; volatile uint32_t var; } r5f_data_t;
typedef struct { volatile uint32_t alive; volatile uint32_t var; } c7x_data_t;
typedef struct { volatile uint32_t alive; volatile uint32_t var; } dsp_data_t;  /* C66x */
typedef struct { volatile uint32_t alive; volatile uint32_t var; } pru_data_t;

/* One frame of system state (generalization of state_t). */
typedef struct {
  volatile uint32_t time;          /* frame counter, stamped by RT island         */
  a72_data_t  a72;
  r5f_data_t  r5f0;
  c7x_data_t  c7x0;
  dsp_data_t  c66x0;
  pru_data_t  pru0;
  pru_data_t  pru1;
} state_t;

/* Parameters (extend as needed). */
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
} param_mem_t;

/* Lookup table (unchanged from today). */
typedef struct { int16_t lut[1000]; } lut_mem_t;

#endif
```

```c
/* mem_types.h — composite layout + ring length. */
#ifndef _MEM_TYPES_
#define _MEM_TYPES_
#include <stddef.h>
#include "state.h"
#include "seqlock.h"

#define STATE_BUFF_LEN 96      /* ring depth (matches today) */

/* The control-plane composite (mapped at OW_CTRL_PHYS_BASE). Field offsets MUST line
 * up with OW_OFF_* in memmap.h — assert this at init (Step 1.6). */
typedef struct {
  sys_header_t header;                       /* OW_OFF_HEADER  */
  core_ctl_t   core_ctl[N_CORES];            /* OW_OFF_CORECTL */
  volatile uint32_t cbuff_index;             /* OW_OFF_STATE   */
  state_t      state[STATE_BUFF_LEN];
  /* seqlock/ping-pong descriptors live at OW_OFF_SEQLOCKS (Step 1.7) */
} system_mem_t;

/* Convenience pointer bundle (generalization of pru_mem_t). */
typedef struct {
  system_mem_t* s;   /* control-plane composite                */
  param_mem_t*  p;   /* OW_OFF_PARAM                           */
  lut_mem_t*    l;   /* OW_OFF_LUT                             */
  void*         data;/* OW_DATA_PHYS_BASE (cacheable, later)   */
} sys_mem_t;

#endif
```

---

## Step 1.3 — Data-plane regions (reserve now, populate later)
The data plane is **cacheable**; for v1 we only *reserve* it and define the
cache-maintenance API the producers/consumers will call later. Add to a new
`library/include/cache.h` the per-core maintenance hooks (implemented per toolchain in
Phase 3):
```c
/* cache.h — data-plane coherence helpers (implemented per core in Phase 3). */
void ow_cache_wb(void* addr, size_t len);    /* writeback (producer, after write) */
void ow_cache_inv(void* addr, size_t len);   /* invalidate (consumer, before read) */
void ow_cache_wbinv(void* addr, size_t len);
```
On the A72 these map to `__builtin___clear_cache` / cacheflush; on R5F/C7x/C66x to the
respective CSL/intrinsic cache ops. v1 firmware doesn't touch the data plane, so stubs
are fine for now — but define the API so the contract is fixed.

---

## Step 1.4 — Device-tree carveout
Add the reserved-memory node(s) to `device-tree/k3-j721e-boneai64-openWearable.dts` at the
addresses chosen in Phase 0 (Step 0.6). Use **`no-map`** so Linux holds no cached alias.

```dts
/ {
    reserved-memory {
        #address-cells = <2>;
        #size-cells = <2>;
        ranges;

        ow_ctrl: ow-ctrl@<PHASE0> {       /* CONTROL plane — non-cacheable */
            reg = <0x00 0x<PHASE0_CTRL_BASE> 0x00 0x00100000>;   /* 1 MB */
            no-map;
        };
        ow_data: ow-data@<PHASE0> {        /* DATA plane — cacheable by cores */
            reg = <0x00 0x<PHASE0_DATA_BASE> 0x00 0x04000000>;   /* 64 MB */
            no-map;
        };
    };
};
```
- **Cross-check against the existing carveouts** found in Step 0.6 — no overlaps.
- If MSMC SRAM is used for the control plane instead of DDR, the node form differs
  (MSMC SRAM region rather than a DDR `reg`) — record the exact form in BOARD-TRUTH and
  mirror it here.
- (Optional, decide in Phase 0/2) add properties to stop the kernel auto-booting the
  cores we own, if runtime take-over proves fragile.
- Recompile + reboot per the procedure captured in Step 0.7. Verify the node appears under
  `/proc/device-tree/reserved-memory/` after reboot.

---

## Step 1.5 — Per-core linker `.cmd` address agreement
Each core's firmware must place its view of the planes at the **same physical addresses**.
For v1 we only need the *addresses* fixed; the non-cacheable attribute is enforced by each
core's MMU/MPU/MAR config in Phase 3.
- PRU: extend the existing `apps/<app>/PRU{0,1}_<arch>.cmd` to point its shared-mem
  CREGISTER region at the control plane (today it targets PRU SRAM at `0x00010000`).
- R5F / C7x / C66x: their linker command files (adapted from the TI references found in
  Step 0.5) get a `MEMORY` entry for `OW_CTRL_PHYS_BASE` (and later `OW_DATA_PHYS_BASE`).
- Keep all four numerically equal to `memmap.h`. Add a comment in each `.cmd` pointing
  back to `memmap.h` as the source of truth.

> v1 deliverable for this step is just the **agreed numbers** documented; the actual
> per-core `.cmd` files are created alongside each firmware in Phase 3.

---

## Step 1.6 — ARM mapper: `SysMemMap()` (generalize `PruMemMap`)
Extend `library/src/pru.c` (to become `rproc.c` in Phase 2) with a mapper that points at
the new control plane, validates the header, and exposes the bundle.
```c
int SysMemMap(sys_mem_t* m) {
  int fd = open("/dev/mem", O_RDWR | O_SYNC);              /* O_SYNC => uncached */
  void* base = mmap(NULL, OW_CTRL_SIZE, PROT_READ|PROT_WRITE,
                    MAP_SHARED, fd, (off_t)OW_CTRL_PHYS_BASE);
  /* ... MAP_FAILED check ... */
  m->s = (system_mem_t*)base;
  m->p = (param_mem_t*)((uintptr_t)base + OW_OFF_PARAM);
  m->l = (lut_mem_t*)  ((uintptr_t)base + OW_OFF_LUT);
  /* data plane mapped lazily / when needed (cacheable path) */
  close(fd);
  return 0;
}
```
**A72 initializes the header** (it's the orchestrator): write `magic/version/n_cores/...`,
zero `core_ctl[]`, then later set `enable`. Add **static asserts** that
`offsetof(system_mem_t, core_ctl) == OW_OFF_CORECTL`, etc., so a layout drift fails at
compile time rather than as a runtime heisenbug.

---

## Step 1.7 — Seqlock / ping-pong descriptors (`seqlock.h`)
The lock-free primitives for island⇄satellite hand-off (per `00-OVERVIEW.md` §4). Header-
only so every toolchain can include it. Provide a per-core memory-barrier macro.
```c
/* seqlock.h — single-writer/multi-reader latest-value, lock-free, for the
 * NON-CACHEABLE control plane. No locks, reader never blocks. */
#ifndef _SEQLOCK_H_
#define _SEQLOCK_H_
#include <stdint.h>

/* Per-core full memory barrier. Define OW_BARRIER per toolchain at build time:
 *   A72/R5F (clang/gcc): __sync_synchronize()
 *   C7x  (cl7x):         __se_barrier()/_mfence equivalent  (confirm in CGT docs)
 *   C66x (cl6x):         _mfence()/memory barrier intrinsic (confirm in CGT docs)
 *   PRU  (clpru):        no caches/store-buffer reorder of concern; compiler barrier
 * Fallback (ordering-only): asm volatile("" ::: "memory"). */
#ifndef OW_BARRIER
#define OW_BARRIER() __sync_synchronize()
#endif

typedef struct { volatile uint32_t seq; } seqlock_t;

/* Writer: bracket the payload write. */
static inline void sl_write_begin(seqlock_t* s){ s->seq++; OW_BARRIER(); }  /* -> odd */
static inline void sl_write_end  (seqlock_t* s){ OW_BARRIER(); s->seq++; }  /* -> even */

/* Reader: returns a snapshot seq; retry while odd or changed. Typical use:
 *   uint32_t v; do { v = sl_read_begin(&s); <copy payload>; } while (sl_read_retry(&s,v)); */
static inline uint32_t sl_read_begin(const seqlock_t* s){
  uint32_t v; do { v = s->seq; } while (v & 1u); OW_BARRIER(); return v; }
static inline int sl_read_retry(const seqlock_t* s, uint32_t v){
  OW_BARRIER(); return s->seq != v; }

/* Ping-pong (larger payloads): writer fills buf[1-cur], then publishes cur. */
typedef struct { volatile uint32_t cur; /* 0/1 */ } pingpong_t;
static inline uint32_t pp_write_buf(const pingpong_t* p){ return 1u - p->cur; }
static inline void     pp_publish (pingpong_t* p, uint32_t idx){ OW_BARRIER(); p->cur = idx; }
static inline uint32_t pp_read_buf(const pingpong_t* p){ OW_BARRIER(); return p->cur; }

#endif
```
Place the concrete descriptors (one `seqlock_t` for a future intent vector, one
`pingpong_t` for a future feature window) at `OW_OFF_SEQLOCKS`. v1 exercises them with
dummy payloads to prove correctness.

---

## Step 1.8 — Phase-1 verification (A72-only self-test)
Full cross-core proof is Phases 3–4. Phase 1 proves the *model* on the A72:
1. Build a tiny test (or temporary mode in `main.c`) that `SysMemMap()`s the control plane.
2. Write `magic/version`, read them back — confirms the **uncached carveout maps and is
   addressable** (depends on the Step 1.4 device-tree node existing post-reboot).
3. Run a single-threaded seqlock round-trip and a ping-pong flip against dummy payloads —
   confirms the primitives compile and behave (logic check; true cross-core racing is
   Phase 4).
4. Confirm the `offsetof` static asserts pass (layout matches `memmap.h`).

---

## Phase 1 exit checklist
- [ ] `library/include/memmap.h` created; all `<PHASE0>` addresses filled from BOARD-TRUTH.
- [ ] `state.h` rewritten (core_id_t, sys_header_t, core_ctl_t, per-core data, state_t).
- [ ] `mem_types.h` updated (system_mem_t, sys_mem_t) with `offsetof` static asserts.
- [ ] `seqlock.h` + `cache.h` added.
- [ ] Device-tree `ow_ctrl` (+ `ow_data`) `no-map` carveout added, no collisions,
      recompiled, rebooted, visible under `/proc/device-tree/reserved-memory/`.
- [ ] Per-core `.cmd` address agreement documented (numbers fixed; files land in Phase 3).
- [ ] `SysMemMap()` implemented; A72 self-test maps the plane, R/W header, seqlock +
      ping-pong round-trip pass, static asserts pass.

**Hand-off to Phase 2:** the generalized remoteproc loader consumes the core descriptor
table (from BOARD-TRUTH §0.2/0.3) and brings each core up against *this* memory model,
reproducing the today's-`template` PRU behavior first (regression), then adding R5F → C66x
→ C7x in Phase 3.
