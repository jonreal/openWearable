#!/usr/bin/env bash
#
# build-firmware.sh — reproduce the patched vision_apps firmware fleet for the BeagleBone AI-64
# (TDA4VM/J721E) C7x/TIDL path. Encodes the build that is proven working on the board (resnet18
# runs on the C7x, 52/52 nodes offloaded).
#
# RUN ON AN x86_64 LINUX BUILD HOST — the C7x (cl7x) and C66 (cl6x) compilers have no arm64 build.
# Prereqs: TI PSDK-RTOS 10.01.00.04 + the CGTs installed (see MANIFEST.md). Outputs land in
# third_party/ti/out/. Deploy + run per docs/C7X-TIDL.md §2.3–§4.
#
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="$HERE/out"; mkdir -p "$OUT"

# ── Paths — EDIT to match your install, or export before calling ────────────────────────────────
: "${TI_SDK_HOME:?export TI_SDK_HOME to your PSDK-RTOS + CGT install root (e.g. ~/ti)}"
PSDK_RTOS="${PSDK_RTOS:-$(find "$TI_SDK_HOME" -maxdepth 2 -type d -name 'ti-processor-sdk-rtos-j721e*' 2>/dev/null | sort | tail -1)}"
: "${PSDK_RTOS:?could not locate ti-processor-sdk-rtos-j721e* under $TI_SDK_HOME — set PSDK_RTOS}"
VISION_APPS="${VISION_APPS:-$(find "$PSDK_RTOS" -maxdepth 1 -type d -name 'vision_apps' | head -1)}"
SDK_BUILDER="${SDK_BUILDER:-$VISION_APPS}"        # where make_firmware.sh lives (TI layout: vision_apps/)
PDK="${PDK:-$(find "$PSDK_RTOS" -maxdepth 1 -type d -name 'pdk*' | head -1)}"
C6X_GEN_INSTALL_PATH="${C6X_GEN_INSTALL_PATH:-$(find "$TI_SDK_HOME" -maxdepth 2 -type d -name 'ti-cgt-c6000_*' | sort | tail -1)}"

echo "PSDK_RTOS = $PSDK_RTOS"
echo "VISION_APPS = $VISION_APPS"
echo "PDK = $PDK"
echo "C6X_GEN_INSTALL_PATH = $C6X_GEN_INSTALL_PATH"

# ── Patch 0001 — relax Sciclient_abiCheck (board TIFS speaks ABI 3, PSDK-10.1 fw expects 4) ─────
# Prefer a captured byte-exact .patch if present; otherwise apply the canonical macro change.
ABI_PATCH="$(ls "$HERE"/patches/0001-*.patch 2>/dev/null | head -1 || true)"
if [[ -n "$ABI_PATCH" ]]; then
    echo "Applying captured patch: $ABI_PATCH"
    git -C "$PDK" apply --check "$ABI_PATCH" 2>/dev/null && git -C "$PDK" apply "$ABI_PATCH" \
        || patch -d "$PDK" -p1 -N --reject-file=- < "$ABI_PATCH" || true
else
    HDR="$(find "$PDK" -path '*drv/sciclient*' -name 'sciclient_fmwMsgParams.h' | head -1)"
    : "${HDR:?could not find sciclient_fmwMsgParams.h under $PDK}"
    if grep -qE '#define[[:space:]]+SCICLIENT_FIRMWARE_ABI_MAJOR[[:space:]]*\(3U?\)' "$HDR"; then
        echo "abiCheck relax already applied (ABI_MAJOR=3) — skipping"
    else
        echo "Relaxing SCICLIENT_FIRMWARE_ABI_MAJOR 4 -> 3 in $HDR"
        cp "$HDR" "$HDR.preAbiRelax-bak"
        perl -0pi -e 's/(#define\s+SCICLIENT_FIRMWARE_ABI_MAJOR\s*\()\s*0?x?0*4U?(\s*\))/${1}3U${2}/' "$HDR"
        grep -E 'SCICLIENT_FIRMWARE_ABI_MAJOR' "$HDR" || { echo "ABI relax FAILED — check the macro form"; exit 1; }
    fi
fi

# ── Build flag — ETHFW off for mcu2_0 (no EVM GESI Ethernet card on the AI-64) ───────────────────
export BUILD_ENABLE_ETHFW=no
# TI makefiles don't track the DEFS flip — force mcu2_0's recompile by dropping its stale objects.
find "$VISION_APPS/out" -path '*R5F*mcu2_0*' \( -name 'app_init.obj' -o -name 'main.obj' \) -delete 2>/dev/null || true
find "$VISION_APPS/out" -path '*R5F*' -name 'vx_app_rtos_linux_mcu2_0.out' -delete 2>/dev/null || true

# ── Build the fleet ─────────────────────────────────────────────────────────────────────────────
export C6X_GEN_INSTALL_PATH
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}"     # some hosts need locallibs here (see MANIFEST host notes)
cd "$SDK_BUILDER"
BUILD_EDGEAI=yes BUILD_CPU_C6x_1=yes BUILD_CPU_C6x_2=yes ./make_firmware.sh

# ── Collect the 5 ELFs ──────────────────────────────────────────────────────────────────────────
for core in mcu2_0 mcu2_1 c6x_1 c6x_2 c7x_1; do
    f="$(find "$VISION_APPS/out" -name "vx_app_rtos_linux_${core}.out" -path '*release*' | head -1)"
    if [[ -n "$f" ]]; then cp -v "$f" "$OUT/"; else echo "WARN: ${core} ELF not found"; fi
done
echo "Done. Fleet ELFs in $OUT/ — deploy per docs/C7X-TIDL.md §2.3."
