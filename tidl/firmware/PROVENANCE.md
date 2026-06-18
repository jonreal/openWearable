# tidl/firmware — prebuilt C7x firmware (deploy artifact)

`vx_app_rtos_linux_c7x_1.out` is the **committed, ready-to-deploy** openWearable fleet-free
C7x TIDL firmware. It is checked in (rather than built on demand) because the C7x toolchain is
x86-only — committing the ELF lets the board **deploy by symlink with no build host** (see
`../README.md` §Deploy). Rebuild it with `make -C tidl/` on an x86 host when the source changes.

## Provenance
- **Built by:** `tidl/Makefile` (standalone — `cl7x` only, no fleet/concerto) from `tidl/src/*`
  over **TI PSDK-RTOS 10.01.00.04**, C7000 CGT 4.1.0.LTS.
- **ELF:** ARM/remoteproc-loadable C7x FreeRTOS image, `strip7x`-stripped (12103880 B). Byte-size +
  section-layout identical to the legacy concerto build.
- **Boot:** entry `_c_int00_secure`, loads to the C7x DDR carveout (rsc-table @0xB2100000,
  boot @0xB2200000, code @0xB2604000); IO via the A72↔C7x DDR mailbox @0xB8000000 (non-cacheable).
- **Behavior:** multi-invoke service loop (create-once / run-many).
- **Verified:** BeagleBone AI-64, reproducible across reboots + back-to-back invokes — tiny MLP
  (`tidl/models/`) runs end-to-end, `max|abs err| 0.0449` vs float ref (int8 quant),
  `create_st=0 invoke_st=0 status=0`, `invoke#` 1→2→3 without reboot.

> Update this file's commit alongside the ELF whenever you rebuild (note any source/behavior change).
