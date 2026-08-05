#!/bin/sh
# enable-pwm.sh -- bring up the ADS131M08 CLKIN (EHRPWM2_A on P9.14) for test-ads131m08.
# Run once after each boot, as root. Safe to re-run and to run before/after the app.
#
# The J721E EHRPWM TBCLK is a Linux-owned clock the R5F cannot gate; only a userspace
# pwm "enable" ungates it. But on a cold (fresh-boot) channel the pwm driver's period
# apply can leave TBPRD=0 / counter frozen -- so this script does two things:
#   (1) enable the pwm channel to ungate TBCLK, then
#   (2) program the EPWM2 registers directly for a guaranteed waveform.
# CLKIN = 7.8125 MHz, 50% duty -- in ADS131M08 spec for every PGA gain.

DEV=3020000.pwm                       # EHRPWM2
PAD=0x0011C178                        # P9.14 CTRL_MMR pad
PAD_MODE6=0x00010006                  # PIN_OUTPUT | mux mode 6 (EHRPWM2_A)

# EHRPWM2 registers (base 0x03020000). TBCLK = 125 MHz.
R_TBCTL=0x03020000; R_TBCNT=0x03020008; R_TBPRD=0x0302000A
R_CMPCTL=0x0302000E; R_CMPA=0x03020012; R_AQCTLA=0x03020016

# --- 1. pinmux P9.14 -> EHRPWM2_A (redundant if the DTB already did it) ----------
command -v devmem2 >/dev/null 2>&1 || { echo "error: devmem2 not found" >&2; exit 1; }
devmem2 "$PAD" w "$PAD_MODE6" >/dev/null 2>&1

# --- 2. find the pwmchip backing EHRPWM2 (numbering varies across boots) ---------
CHIP=""
for c in /sys/class/pwm/pwmchip*; do
  [ -e "$c/device" ] || continue
  if [ "$(basename "$(readlink -f "$c/device")")" = "$DEV" ]; then CHIP="$c"; break; fi
done
[ -n "$CHIP" ] || { echo "error: pwmchip for $DEV not found (is &main_ehrpwm2 enabled in the DTB?)" >&2; exit 1; }

# --- 3. export channel 0 (EHRPWM2_A) and wait for its sysfs attrs (udev) ---------
if [ ! -d "$CHIP/pwm0" ]; then
  echo 0 > "$CHIP/export"
  i=0; while [ ! -w "$CHIP/pwm0/period" ] && [ "$i" -lt 25 ]; do sleep 0.2; i=$((i + 1)); done
fi
[ -w "$CHIP/pwm0/period" ] || { echo "error: $CHIP/pwm0 never appeared after export" >&2; exit 1; }

# --- 4. enable the channel -> ungates TBCLK (needs a nonzero sysfs period first) --
echo 128 > "$CHIP/pwm0/period"     2>/dev/null   # 7.8125 MHz
echo 64  > "$CHIP/pwm0/duty_cycle" 2>/dev/null   # 50 %
echo 1   > "$CHIP/pwm0/enable"

# --- 5. program EPWM2 directly: TBPRD=15 -> 125MHz/16 = 7.8125 MHz, CMPA=8 (~50%) -
devmem2 "$R_TBCTL"  h 0x0003 >/dev/null   # freeze while configuring
devmem2 "$R_TBCNT"  h 0x0000 >/dev/null
devmem2 "$R_TBPRD"  h 0x000F >/dev/null
devmem2 "$R_CMPCTL" h 0x0000 >/dev/null   # CMPA shadow-loads at CTR=0
devmem2 "$R_CMPA"   h 0x0008 >/dev/null
devmem2 "$R_AQCTLA" h 0x0012 >/dev/null   # set@zero, clear@CMPA
devmem2 "$R_TBCTL"  h 0x0000 >/dev/null   # up-count -> run

# --- 6. verify the counter is actually clocking ---------------------------------
A=$(devmem2 "$R_TBCNT" h 2>/dev/null | grep -o '0x[0-9A-Fa-f]*$')
B=$(devmem2 "$R_TBCNT" h 2>/dev/null | grep -o '0x[0-9A-Fa-f]*$')
C=$(devmem2 "$R_TBCNT" h 2>/dev/null | grep -o '0x[0-9A-Fa-f]*$')
echo "CLKIN on $CHIP/pwm0  (P9.14, EHRPWM2_A, mode 6):"
echo "  pad $PAD = $(devmem2 "$PAD" w 2>/dev/null | grep -o '0x[0-9A-Fa-f]*$') (expect 0x00010006)"
echo "  TBPRD = $(devmem2 "$R_TBPRD" h 2>/dev/null | grep -o '0x[0-9A-Fa-f]*$') (expect 0x000F -> 7.8125 MHz)"
echo "  TBCNT = $A $B $C"
if [ "$A" = "$B" ] && [ "$B" = "$C" ]; then
  echo "  WARNING: TBCNT not advancing -- CLKIN may be dead (check wiring / re-run)." >&2
else
  echo "  OK: counter running, ~7.8125 MHz on P9.14."
fi
