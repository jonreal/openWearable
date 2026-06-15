# openWearable library — layout

Per-core source/include separation, so each compute core builds with its own
toolchain against a single shared memory contract.

| Dir       | Core / role                                   | Compiler        | Output |
|-----------|-----------------------------------------------|-----------------|--------|
| `common/` | The cross-core contract (inter-core ABI): `mem_types.h`, `fix16.h`, `int64.h`. **One source of truth** — included by every core. | — | headers only |
| `a8/`     | Cortex-A8 / Linux app core: remoteproc loader, UI, logging, sensors. | `gcc` (per app) | linked into the app binary |
| `pru/`    | PRU-ICSS firmware: real-time loop + drivers + filters. | `clpru` | `lib/libow_pru.a` (drivers) + per-app `pru{0,1}_main` |

Each core dir holds `include/` and `src/`. Linker command (`.cmd`) files live with
each **app**, not here — memory layouts are app-specific (`apps/<app>/PRU{0,1}_<arch>.cmd`).

The library Makefile builds the per-core archive (`libow_pru.a`); apps add their
per-core `*loop.c` hooks and link against it. Build the library **first**:

```bash
cd library && make          # -> library/lib/libow_pru.a
cd ../apps/template && make  # -> A8 binary (apps/bin/) + PRU firmware (/lib/firmware/)
```

> This is the `master` branch (BeagleBone Black/Blue, AM335x: Cortex-A8 + 2× PRU).
> The `am64x` branch carries the same layout for the BeagleBone AI-64 (TDA4VM/J721E),
> where `a8/` becomes `a72/` and the split extends to `r5f/`, `c7x/`, and `c66x/`.
