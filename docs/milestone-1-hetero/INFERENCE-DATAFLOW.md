# EMG inference dataflow & shared-memory buffer map

How the EMG transformer pipeline plugs into the two-plane memory model (see `00-OVERVIEW.md`
§3–4). This is the layout we build against. **Route is locked** (decisions below).

## Locked decisions
- **C7x = TIDL only, never bare-metal.** No `cl7x`, no hand-written C7x kernels. The C7x is a
  black-box accelerator behind a runtime; the **A72 is its producer-proxy** in our mesh.
  → toolchain: `edgeai-tidl-tools` on an **x86 host** to *compile* the model (one-time);
  the **prebuilt TIDL runtime on the board** executes it. (No `cl7x`/MCU+ SDK needed.)
- **A72 inference code stays C.** Use a **C API** runtime — **ONNX Runtime C API**
  (`onnxruntime_c_api.h`, TIDL EP appendable from C; matches our ONNX model) or **Neo-AI-DLR**
  (`dlr.h`, cleanest pure-C, via the TVM compile path). The app `.c` links the C++-built
  runtime `.so` transparently; no C++ in our code.
- **C66x preprocessing = bare-metal, *optional*.** EDMA-streamed classical DSP is the C66x's
  wheelhouse and is more tractable than bare-metal C7x would've been — but it's still an
  x86-host `cl6x` build + resource table + IPC. Add it only if preprocessing is heavy
  (filter banks / spectral); otherwise do light preprocessing on A72/PRU or fold it into the
  ONNX graph.
- **Inference is an async satellite.** It never sits in the >kHz control loop; the loop only
  reads the latest cached intent. Inference rate ~100–500 Hz is fine (EMG dynamics are slow).

## Pipeline
```
EMG acquire (PRU/R5F, or DMA from ADC) ─▶ emg_raw ring         [DATA plane, cacheable/DMA]
                                              │
        preprocess:  C66x (bare-metal)  OR  A72/NEON  OR  folded into ONNX graph
                                              ▼
                                         feature window         [DATA plane, cacheable]
                                              │  A72: invalidate + read
                                              ▼
        A72 ─▶ ONNX-RT/DLR (C API) + TIDL ─▶ C7x+MMA ─▶ tidl_out  [DATA plane, runtime-managed]
                                              │  A72: read result, post-process
                                              ▼  (the plane bridge: cached → non-cached copy)
                                         intent vector          [CONTROL plane, NON-cached, seqlock]
                                              │
                       PRU / R5F read latest intent @ >kHz      ◀── unchanged, deterministic
```

## Buffer map
| Buffer | Plane | Cacheable | Producer | Consumer | Cache op at hand-off |
|--------|-------|-----------|----------|----------|----------------------|
| `emg_raw` (sample ring) | DATA | yes (or DMA region) | PRU/R5F acquire, or EDMA from ADC | preprocessor | producer writeback / **DMA → consumer invalidate before read** |
| `feature_win` (model input) | DATA | yes | C66x *or* A72 *or* (none, if folded into ONNX) | A72 (feeds runtime) | producer writeback; A72 invalidate before read (no-op if A72 produced it) |
| `tidl_in` / `tidl_out` | DATA | yes (runtime-managed) | A72 ↔ TIDL/C7x | A72 ↔ TIDL/C7x | handled by the OSRT/TIDL runtime |
| **`intent`** (compact result) | **CONTROL** | **no** | **A72** (post-inference) | **PRU / R5F @ >kHz** | **none** — non-cached + **seqlock** |
| `core_ctl[]`, `state[]`, params | CONTROL | no | various | various | none — non-cached + seqlock/barrier |

**Key point on the "plane bridge":** the small `intent` vector that the control loop consumes
lives in the **non-cached control plane** (so the PRU/R5F read it coherently with zero cache
ops). The A72 reads the (cached, runtime-managed) `tidl_out`, post-processes, and **copies the
compact result into the non-cached `intent` slot** — that copy *is* the cached→non-cached
bridge. It's tiny, so the copy is free. (Don't try to alias across planes — different
cacheability.)

## Cache-coherence rules (recap)
- **DATA plane (cacheable):** producer **writeback** after writing; consumer **invalidate**
  before reading. DMA into a cached buffer ⇒ **invalidate before the CPU reads it**.
- **CONTROL plane (non-cached):** nothing — `volatile` + seqlock/barrier is sufficient (the
  PRU is cacheless; the A72 maps it `O_SYNC`; R5F/C7x via MPU/MAR non-cacheable).

## Build / run split (recap)
- **x86 host (one-time per model):** `edgeai-tidl-tools` compiles ONNX → TIDL artifacts.
- **board:** prebuilt TIDL runtime runs inference (and benchmarks) on the C7x+MMA via the
  ONNX-RT/DLR **C API**; the A72 publishes `intent` into the control plane.
- **board (native):** PRU (`clpru`), A72 (`gcc`), R5F (`gcc-arm-none-eabi`) build as usual.
- **x86 host (only if used):** bare-metal C66x via `cl6x` + MCU+ SDK.

See `C7X-GETTING-STARTED.md` for the C7x/TIDL specifics and the empirical offload-report gate.
