# template — multi-core app skeleton (starting point)

The reference app and **the starting point for any new app**: brings up one of each
core in the `Init / Update / Cleanup` lifecycle — **PRU0, PRU1, R5F, A72** — with
optional **C7x + MMA** neural-net inference. Copy it and fill in the per-core hooks.

## What it does
- **PRU0/PRU1, R5F, A72** — each core runs the lifecycle and just increments its own
  counter (`pru0var`, `pru1var`, `r5fvar`). **No peripherals are wired** — it's a
  pure skeleton; add device bring-up in the hooks (a `test-spi` app comes later).
- **C7x** (optional) — runs a TIDL MLP (16 in → 8 out); the A72 bridges features in
  and predictions out.

## Run
```sh
cd library && make
cd apps/template && make
apps/bin/template                                         # cores only, NN off
apps/bin/template -n ow_tiny_net.bin -i ow_tiny_io.bin    # + C7x inference
```
- `-n <net.bin>` — C7x TIDL model  ·  `-i <io.bin>` — tensor/IO descriptor
- **`-n` requires `-i`.** Without them, NN is disabled and the cores still run.
- Ships `ow_tiny_net.bin` + `ow_tiny_io.bin`.  (`-v` debug rows, `-s` stats.)

## Use as a starting point
Copy `apps/template/` → `apps/<yourapp>/` and edit the hooks:

| File | Core | Fill in |
|------|------|---------|
| `pruloop.c` | PRU0/1 | real-time I/O + control |
| `r5floop.c` | R5F | real-time compute |
| `cpuloop.c` | A72 | orchestration |
| `nnloop.c` | C7x | model features / how the output is used |
| `uiloop.c` | A72 | menu, logging, `PruLoadParams` |
| `state.h` / `format.c` | — | shared-mem layout + log schema |
