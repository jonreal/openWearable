# C7x — toolchain reality & programming-model primer

Research notes to get us started on the C7x (the deep-learning core on the TDA4VM, exposed
as remoteproc `64800000.dsp` / `j7-c71_0`). Two questions: **how do we build for it**, and
**how do we program it (with and without the MMA)**. Sources at the bottom.

> **Bottom line up front:** the C7x is the hardest core to bring up — its compiler is
> **x86_64-host only** (no native arm64, so it breaks our "build on the board" rule), it
> needs the TI SDK's startup + resource tables, and it has a genuinely new programming
> model (VLIW + streaming engine + MMA). But it's tractable with the right sequencing, and
> for a *lightweight* EMG transformer **you likely never touch the MMA** — which removes the
> hardest part.

---

## 1. Toolchain reality (the build-host decision, revised)

| Core | Compiler | Native arm64 (on-board)? | How we build |
|------|----------|--------------------------|--------------|
| PRU  | `clpru`  | ✅ yes (apt `ti-pru-cgt`) | on the board |
| A72  | `gcc`    | ✅ yes (native Linux) | on the board |
| R5F  | `arm-none-eabi-gcc` | ✅ yes (apt `gcc-arm-none-eabi`) | on the board |
| **C7x** | **`cl7x`** | ❌ **no** | **x86_64 Linux host → copy ELF to board** |
| C66x | `cl6x`   | ❌ no (x86_64, or fragile 32-bit armhf) | x86_64 Linux host |

**Confirmed from TI's download page:** C7000 CGT **5.0.0.LTS** ships exactly two installers —
`ti_cgt_c7000_5.0.0.LTS_linux-x64_installer.bin` (x86_64) and `…_windows-x64_installer.exe`.
**There is no aarch64/arm64 build of `cl7x`.** Same story for `cl6x` (C6000 archive lists
x86_64, 32-bit ARM-A8/A15, Windows, macOS — no aarch64).

**Consequence:** our "everything native on the board" rule holds for PRU + A72 + R5F, but
**the C7x (and C66x) firmware must be cross-built on an x86_64 Linux machine** (PC, VM, or
container) and the resulting ELF copied to the board's `/lib/firmware/` — remoteproc doesn't
care where the ELF came from, so the runtime architecture is unchanged; only the C7x *build
step* moves off-board. (Running x86 `cl7x` on the arm64 board via qemu/box64 is technically
possible but slow/fragile — not recommended.)

### What to install on the x86_64 build host
- **C7000 CGT** (`cl7x`) — pick a version that matches the SDK below. (TI's BeagleY-AI
  walkthrough used `ti-cgt-c7000_4.1.0.LTS`; 5.0.0.LTS is current.)
- **TI MCU+ SDK** (or PROCESSOR-SDK-RTOS) **for J721E** — provides C7x device startup, linker
  command files, the **resource table** (remoteproc needs it — TI's SDK examples sometimes
  *omit* it; their prebuilt firmware includes it), plus **MMALIB** and **TIDL**.
- For C66x later: **C6000 CGT** (`cl6x`, v8.3.x).

> The C7x ELF reports as `unknown arch 0x91` to `file(1)` — that's the C7x machine type, not
> an error.

---

## 2. The programming model (mental map)

The C7x is a **VLIW scalar + vector DSP** (the TDA4VM part is a C71x @ ~1 GHz, ~80 GFLOP/s
FP / ~256 GOPS), with a bolt-on **MMA** for dense matrix multiply (~8 TOPS INT8). Three
things to internalize:

1. **VLIW + wide vectors.** ~13 fully-pipelined functional units issue up to ~13 ops/cycle;
   vector (SIMD) instructions do up to **64 operations each** (512-bit vectors — e.g. 64×INT8
   or 16×FP32). You get throughput by *vectorizing* (vector data types / intrinsics, or
   compiler auto-vectorization) and keeping the pipeline fed. This is the part that feels
   like classic DSP/SIMD.
2. **Streaming Engine (SE).** Two streaming engines (SE0/SE1) + four streaming address
   generators (SA0–3) **stream structured data (matrix/tensor tiles) from L2 into the
   functional units** along a described access pattern (a "stream template"). Efficient C7x
   code is built around the SE feeding vectors, *not* hand-written load loops. This is the
   part that's genuinely new vs. a normal CPU.
3. **MMA = a separate accelerator driven *from* a C7x program.** It reuses the C7x's
   streaming engine / registers / L1-L2. You set it up and fire matmuls via intrinsics in
   `c7x_mma.h`. So "C7x + MMA" = a C7x program that streams operands and issues MMA ops.

### Three abstraction levels for *our* transformer
| Level | What you write | MMA use | Tradeoff |
|-------|----------------|---------|----------|
| **TIDL** | export ONNX/TFLite → TIDL compile → run via runtime | automatic | least effort, but heavy stack + dispatch latency; transformer-op coverage is version-dependent |
| **MMALIB** | your own C7x program calling TI's optimized matmul/CNN/FFT primitives (`dsp_c7xmma…`) | via the lib | mid effort; you own orchestration, TI owns the hot kernels |
| **Hand intrinsics** | forward pass in C7x C with vector + SE + `c7x_mma.h` intrinsics | manual | max control / determinism, steepest curve |

**Key de-risking fact:** the C7x *core alone* (vectorized FP via `cl7x`, **no MMA**) is
~80 GFLOP/s. A lightweight EMG transformer is single-digit-to-tens of MFLOP/inference → a
few GFLOP/s even at high rate. So we can almost certainly run it **FP on the C7x core
without the MMA** — skipping INT8 quantization *and* the MMA intrinsics entirely. Adopt the
MMA only if the model grows enough to need the 8 TOPS.

---

## 3. Reading list (start here, in order)
- **SPRUIV4** — *C7000 C/C++ Optimization Guide*. The practical programming guide
  (architecture, streaming engine, vectorization, worked examples). **Read first.**
- **SPRUIG8** — *C7000 Optimizing C/C++ Compiler User's Guide* (`cl7x` usage; intrinsics incl.
  the MMA intrinsics, §5.15.5).
- **SPRUIP0** — *C71x DSP CPU, Instruction Set, and MMA Technical Reference Manual* (the
  hardware bible; consult for MMA details).
- **SPRUIG6** — *C7000 Host Emulation User's Guide* — lets you compile/run C7x intrinsic code
  on a **PC** for development/testing (no board needed to iterate on kernel logic).
- **Olof Astrand, "Programming the BeagleY-AI C7x256 … using the cl7x compiler"** — the
  closest hands-on precedent (arm64 BeagleBone-class board, remoteproc load, the
  resource-table gotcha). Read for the end-to-end mechanics.
- **MMALIB** docs + the `vision_apps` `app_c7x_kernel` example — the simplest "call a matmul
  on the C7x/MMA" reference. **TIDL** docs if we go the NN-graph route.

---

## 4. Recommended sequence (de-risked)
1. **Prove the pipeline on the A72 first.** Run the EMG transformer on Linux (ONNXRuntime CPU
   or small hand C / NEON). Validates EMG → features → inference → control end-to-end and
   gives a real FLOP/latency budget — **no C7x toolchain needed.** This may even be the *v1
   inference home*, with the C7x as a deliberate later offload (the async-satellite design
   means inference rate is decoupled from the >kHz control loop either way).
2. **Stand up the x86_64 build host** (cl7x + J721E MCU+ SDK). Build TI's C7x `hello_world`,
   add/borrow a resource table, copy the ELF to the board, load via remoteproc, confirm it
   runs and can exchange data with the A72. This is "C7x hello-world" — de-risks toolchain +
   boot independently of the NN.
3. **Port the forward pass to the C7x**, FP first (C7x core, vectorized, MMALIB matmul where
   it helps). Quantize to INT8 + adopt the MMA *only if* the FLOP budget demands it.

**Impact on Milestone 1:** the heterogeneous template's C7x firmware will be built off-board
(x86) and dropped into `/lib/firmware/`; PRU/R5F still build on-board. The generic remoteproc
loader is agnostic to where the ELF came from, so the milestone architecture is unaffected —
only the C7x build step lives elsewhere.

---

## Sources
- C7000-CGT download (host installers): https://www.ti.com/tool/download/C7000-CGT/5.0.0.LTS
- C7000 CGT product page: https://www.ti.com/tool/C7000-CGT
- C7000 Optimization Guide (architecture, streaming engine): https://software-dl.ti.com/codegen/docs/c7000/optimization_guide/2_architecture/compopt_arch_overview.html
- Streaming Engine: https://software-dl.ti.com/codegen/docs/c7000/optimization_guide/6_adv_code_opt/compopt_adv_streaming_engine.html
- MMA intrinsics (SPRUIG8 §5.15.5): https://downloads.ti.com/docs/esd/SPRUIG8/matrix-multiply-accelerator-mma-intrinsics-t5014531-165.html
- C7000 Optimization Guide PDF (SPRUIV4): https://www.ti.com/lit/pdf/spruiv4
- MMALIB / MMA-on-C7x (E2E): https://e2e.ti.com/support/processors-group/processors/f/processors-forum/903670/tda4vm-mma-on-c7x
- BeagleY-AI cl7x walkthrough (Olof Astrand): https://olof-astrand.medium.com/programming-the-beagley-ai-c7x256-coprocessors-using-the-cl7x-compiler-8ab814c272eb
- Processor SDK RTOS J721E components: https://software-dl.ti.com/jacinto7/esd/processor-sdk-rtos-jacinto7/latest/exports/docs/psdk_rtos/docs/user_guide/sdk_components_j721e.html
