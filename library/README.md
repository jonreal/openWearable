# openWearable library — layout

Per-core source/include separation, so each compute core builds with its own
toolchain against a single shared memory contract.

| Dir       | Core / role                                   | Compiler        | Output |
|-----------|-----------------------------------------------|-----------------|--------|
| `common/` | The cross-core contract (inter-core ABI): `mem_types.h`, `fix16.h`, `int64.h`. **One source of truth** — included by every core. | — | headers only |
| `a72/`    | Cortex-A72 / Linux app core: remoteproc loader, UI, logging, sensors. | `gcc` (per app) | linked into the app binary |
| `pru/`    | PRU-ICSSG firmware: real-time loop + drivers + filters. | `clpru` | `lib/libow_pru.a` (drivers) + per-app `pru{0,1}_main` |
| `r5f/`    | Cortex-R5F real-time control. *(added in Phase 3)* | `arm-none-eabi-gcc` / `tiarmclang` | per-core archive |
| `c7x/`    | C7x DSP + MMA: ML inference. *(added in Phase 3)*  | `cl7x` | per-core archive |
| `c66x/`   | C66x DSP: classical DSP front-end. *(added in Phase 3)* | `cl6x` | per-core archive |

Each core dir holds `include/` and `src/`. Linker command (`.cmd`) files live with
each **app**, not here — memory layouts are app-specific.

The library Makefile builds the per-core archives; apps add their per-core
`*loop.c` hooks and link only what they use. See `docs/C7X-TIDL.md` for the C7x bring-up
this layout is built toward.
