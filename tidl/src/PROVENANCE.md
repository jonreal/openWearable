# tidl/src — provenance

These are the four sources the Makefile compiles into the C7x firmware's "application" objects.
Two are openWearable's own; two are TI PSDK-RTOS 10.01.00.04 files we vendor **because we modify
them** (so there is no copy-from-SDK + patch step — the file here is the final source).

| File | Origin | openWearable change |
|---|---|---|
| `ow_c7x_tidl.c` | openWearable | — the platform layer: scratch pools, the 32 MB persistent-alloc arena, the A72↔C7x mailbox, and `ow_c7x_tidl_run()` (the create/invoke service loop). **Runtime I/O sizing:** the per-model input/output element counts arrive over the mailbox (`OW_MB_INLEN`/`OW_MB_OUTLEN` @ `0x78`/`0x7C`) instead of being compile-time `OW_N_IN`/`OW_N_OUT`, so a new model size needs only a fresh `net.bin`/`io.bin` + reboot — no firmware rebuild. `s_in`/`s_out` are sized to `OW_N_*_MAX` (**1024** — a 16384/64 KB buffer was HW-proven to hard-stall `TIDLRT_invoke`; 1024 builds identical to the proven 16/8 image) and the runtime count is bounds-checked; `0` falls back to the legacy 16/8 contract. |
| `platform_defines.h` | openWearable | — build config; `L4_MEM_SIZE` MUST equal `s_l4[]` in `ow_c7x_tidl.c` (24 MB). |
| `main.c` | PSDK `vision_apps/platform/j721e/rtos/c7x_1/main.c` | In `appMain`, replaced the fleet chain `appInit(); appRun();` with `ow_c7x_tidl_run()` (keeping the `appGetIpcResourceTable()` reference so `.resource_table` is retained); and remapped `DDR_SHARED_MEM` **non-cacheable** (MAIR4) for the A72 `/dev/mem` mailbox. |
| `tidl_rt.c` | PSDK `c7x-mma-tidl/ti_dl/test/src/tidl_rt.c` | **The fix:** TI's test harness wired the engine's `pFxnUnLock` to `memset(0xDA)` the L1/L2/L3 scratch on every critical-section exit ("code to corrupt buffers"), corrupting the live MMA/DRU stream mid-`algProcess`. Restored real `HwiP_disable/restore` and removed the memset (mirrors the production `vx_tidl_target.c`). See `notes/milestone-1-hetero/C7X-TIDL-RT-RECON.md`. |

Two more single-file TI helpers are compiled **from the SDK in place** (unmodified, so not vendored):
`ti_mem_manager.c` (`c7x-mma-tidl/arm-tidl/rt/test/src/`) and `ipc_trace.c`
(`vision_apps/platform/j721e/rtos/`). The C71/FreeRTOS `app_utils` sources are likewise compiled
from the SDK by the Makefile (see it for the exact list). TI files keep their original BSD-3-Clause
headers.
