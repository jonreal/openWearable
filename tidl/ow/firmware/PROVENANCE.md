# tidl/ow/firmware — prebuilt C7x firmware (deploy artifacts)

This dir holds **two** committed, ready-to-deploy openWearable fleet-free C7x TIDL firmwares. They
are checked in (rather than built on demand) because the C7x toolchain is x86-only — committing the
ELF lets the board **deploy by symlink with no build host** (see `../README.md` §Deploy). Rebuild
with `make -C tidl/ow/` on an x86 host when the source changes.

| File | I/O sizing | sha256 (head) | Bytes |
|---|---|---|---|
| `vx_app_rtos_linux_c7x_1.out` | **variable, ≤1024** in/out (runtime `INLEN`/`OUTLEN`) | `f7fdfa27` | 12103880 |
| `vx_app_rtos_linux_c7x_1.out.fixed-16x8` | fixed **16→8** (compile-time) | `5865138c` | 12103880 |

Both are byte-size-identical: a 1024-float `s_in`/`s_out` does not grow the image vs the original
16/8 buffers. The bare name is the `make` output and the recommended deploy; `.fixed-16x8` is the
prior image, frozen for rollback + provenance.

## Provenance (both)
- **Built by:** `tidl/ow/Makefile` (standalone — `cl7x` only, no fleet/concerto) from `tidl/ow/src/*`
  over **TI PSDK-RTOS 10.01.00.04**, C7000 CGT 4.1.0.LTS. The build is reproducible (no embedded
  timestamps): rebuilding `vx_app_rtos_linux_c7x_1.out` from the committed source reproduces
  `f7fdfa27` exactly.
- **ELF:** ARM/remoteproc-loadable C7x FreeRTOS image, `strip7x`-stripped. Byte-size +
  section-layout identical to the legacy concerto build.
- **Boot:** entry `_c_int00_secure`, loads to the C7x DDR carveout (rsc-table @0xB2100000,
  boot @0xB2200000, code @0xB2604000); IO via the A72↔C7x DDR mailbox @0xB8000000 (non-cacheable).
- **Behavior:** multi-invoke service loop (create-once / run-many).

## The variable-size image (`vx_app_rtos_linux_c7x_1.out`)
- Per-model input/output element counts arrive at runtime over the mailbox (`OW_MB_INLEN`/
  `OW_MB_OUTLEN`); a new model of I/O ≤ 1024 needs only a fresh `net.bin`/`io.bin` + reboot — no
  firmware rebuild. `0` (legacy host) falls back to the fixed 16→8 contract.
- **`OW_N_*_MAX` is 1024 deliberately.** A 16384-float (64 KB) `s_in`/`s_out` was validated on
  hardware to **hard-stall `TIDLRT_invoke`** (`create_st=0` but `algProcess` never returns;
  `DONE`/`STATUS`/`INVOKES` never written) — a large contiguous cacheable I/O buffer trips an
  engine/erratum assumption. At 1024 the image is byte-size-identical to the proven 16/8 build and
  runs clean. **Raising the cap requires re-validation on the board, not just a recompile.**
- **Verified:** BeagleBone AI-64 (`boneai64-0`), first invoke after a warm reboot — default 16/8
  tiny MLP (`tidl/ow/models/ow_tiny_{net,io}.bin`) runs end-to-end, `max|abs err| 0.0449` vs float ref
  (int8 quant), `create_st=0 invoke_st=0 status=0 invoke#=1`.

## The legacy image (`vx_app_rtos_linux_c7x_1.out.fixed-16x8`)
- The first hardware-proven fleet-free C7x TIDL image; fixed 16→8 (compile-time `s_in[16]`/`s_out[8]`).
- **Verified:** BeagleBone AI-64, reproducible across reboots + back-to-back invokes — tiny MLP
  runs end-to-end, `max|abs err| 0.0449`, `create_st=0 invoke_st=0 status=0`, `invoke#` 1→2→3.

> Update this file alongside the ELFs whenever you rebuild (note any source/behavior change).
