# Firmware patches

These are the source-level patches the firmware fleet needs (beyond the in-tree device tree and
the apt-installed kernel). Each is small and reversible. `build-firmware.sh` applies any
`*.patch` here; for the ABI-check relax it also has a robust scripted fallback so the build works
before you've captured the byte-exact diff (see below).

---

## 0001 — relax `Sciclient_abiCheck` (ABI-major skew)

**Why.** The board's stock Device Manager (`Fiery Fox 10.0.1`) speaks **TISCI ABI major 3**;
PSDK-10.1's Sciclient hardcodes an expected major of **4**. The transport works fine on ABI 3
(the R5F does UDMA and thousands of DM ping-pongs without issue) — only the strict equality check
fails, returning `CSL_EFAIL` and hanging the firmware at `appSciclientInit` (`while(1)` assert).
This blocks **every** core, so the relax must apply to all of them (an early attempt that guarded
it to the DSPs left the R5Fs hung).

**The change.** Two equivalent expressions; pick one:

- **(canonical, robust)** Lower the expected major from 4 to 3 — a single token, in
  `pdk .../drv/sciclient/soc/V1/sciclient_fmwMsgParams.h`:
  ```c
  #define SCICLIENT_FIRMWARE_ABI_MAJOR   (3U)   /* was (4U): match the board's stock TIFS ABI 3 */
  ```
- **(what the board firmware was first proven with)** Drop the `abi_major` term from the check in
  `pdk .../drv/sciclient/src/sciclient.c`, `Sciclient_abiCheck()`, so any ACK'd version response
  passes (keep the `abi_minor` check).

`build-firmware.sh` applies the canonical macro change in-place (idempotent, guarded) if no
`0001-*.patch` file is present here.

**Capture the byte-exact patch (recommended, on the build host).** Once the SDK source is checked
out and edited, snapshot the real diff so review/repro is exact:
```bash
PDK=$(find "$TI_SDK_HOME" -type d -name 'pdk*' | head -1)
git -C "$PDK" diff -- packages/ti/drv/sciclient \
  > third_party/ti/patches/0001-sciclient-abicheck-relax.patch
```
(If the PDK isn't a git repo, `diff -u` the pristine vs edited file instead.) Commit the captured
`.patch`; the script will then prefer it over the scripted edit.

---

## Build flag (not a patch): `BUILD_ENABLE_ETHFW=no`

mcu2_0 is the ethfw core; `appEthFwInit` → `EthFwBoard_init(ETHFW_BOARD_GESI_ENABLE)` expects the
EVM's GESI Ethernet card, which the AI-64 lacks, so it asserts/hangs. We build mcu2_0 with ETHFW
off. This is an env flag (`vision_apps_build_flags.mak: BUILD_ENABLE_ETHFW ?= yes`), set in
`build-firmware.sh` — not a source patch. TI makefiles don't track the DEFS flip, so the script
removes mcu2_0's stale objects to force the recompile.
