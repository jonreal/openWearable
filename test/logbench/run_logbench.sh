#!/usr/bin/env bash
# Logging throughput benchmark sweep for openWearable (test/logbench).
#
# Ramps the PRU sample rate (fs) and, at each step, runs the *whole* OW stack
# (PRU + A8 + ring/thread logger) logging for a fixed duration, then reports
# whether the A8 kept up:
#   drops   - log-ring overflow: the writer thread / disk couldn't drain the
#             4 MB RAM ring (reported by the logger on close).
#   cap%    - records actually written vs fs*secs. A shortfall means the PRU
#             state-ring (STATE_BUFF_LEN deep, drained by the fixed ~200 Hz A8
#             SIGALRM timer) overflowed upstream of the log ring.
# The limit is the highest fs with no drops and ~100% capture. With
# STATE_BUFF_LEN=96 and a 200 Hz drain, the A8 cadence ceiling is ~19.2 kHz
# (96 / 0.005 s) before sample loss — well below any eMMC bandwidth limit.
#
# Usage (on the board):
#   test/logbench/run_logbench.sh
# Env overrides:
#   SECS=8                       run length per fs (s)
#   FREQS="1000 ... 50000"       sample rates to sweep (Hz)
#   SINK=/root/openWearable/test/datalog   log dir (set SINK=/dev/shm for tmpfs)
#   KEEP=1                       keep the per-fs log files (default: delete)
#   SKIP_BUILD=1                 don't rebuild library + logbench first
set -u

OW_DIR=${OW_DIR:-/root/openWearable}
BIN=$OW_DIR/test/bin/logbench
CFG_DIR=$OW_DIR/apps/config         # must match the hardcoded -c prefix in main.c
CFG=$CFG_DIR/.logbench              # transient config (first token = fs in Hz)
RECBYTES=16                         # template record: time,pru0,pru1,cpu = 4 x u32

SECS=${SECS:-8}
SINK=${SINK:-$OW_DIR/test/datalog}
KEEP=${KEEP:-0}
FREQS=${FREQS:-"1000 2000 5000 10000 15000 18000 19000 20000 25000 50000"}

mkdir -p "$SINK"
trap 'rm -f "$CFG"' EXIT

# Build (library first, then logbench) unless told to skip.
if [ "${SKIP_BUILD:-0}" != "1" ]; then
  echo "Building libow_pru + logbench ..."
  if ! ( cd "$OW_DIR/library" && make ) >/tmp/logbench_build.log 2>&1; then
    echo "library build FAILED:"; tail -25 /tmp/logbench_build.log; exit 1
  fi
  if ! ( cd "$OW_DIR/test/logbench" && make ) >>/tmp/logbench_build.log 2>&1; then
    echo "logbench build FAILED:"; tail -25 /tmp/logbench_build.log; exit 1
  fi
fi

[ -x "$BIN" ] || { echo "missing binary: $BIN (build failed?)"; exit 1; }

echo
echo "fs sweep: secs=$SECS sink=$SINK"
printf "%-9s %-12s %-12s %-7s %-7s %s\n" "fs(Hz)" "expect" "captured" "drops" "cap%" "verdict"
printf -- "----------------------------------------------------------------\n"

for fs in $FREQS; do
  printf "%d\t// Freq (logbench sweep)\n" "$fs" > "$CFG"
  file="$SINK/logbench_${fs}hz.owl"
  rm -f "$file"

  out=$(LOGBENCH_SECS="$SECS" LOGBENCH_SINK="$SINK" \
        "$BIN" -s -c "$(basename "$CFG")" </dev/null 2>&1)

  if [ ! -f "$file" ]; then
    printf "%-9d %-12s %-12s %-7s %-7s %s\n" "$fs" "-" "-" "-" "-" "NO-FILE"
    continue
  fi

  # Data starts right after the "#DATA\n" sentinel line.
  hdroff=$(grep -abom1 '^#DATA$' "$file" | cut -d: -f1)
  total=$(stat -c %s "$file" 2>/dev/null || echo 0)
  if [ -z "$hdroff" ]; then
    databytes=0
  else
    databytes=$(( total - hdroff - 6 ))      # 6 = len("#DATA\n")
    [ "$databytes" -lt 0 ] && databytes=0
  fi
  captured=$(( databytes / RECBYTES ))
  expected=$(awk "BEGIN{printf \"%d\", $fs*$SECS}")
  drops=$(printf '%s\n' "$out" | sed -n 's/.*: \([0-9][0-9]*\) records dropped.*/\1/p' | tail -1)
  drops=${drops:-0}
  cappct=$(awk "BEGIN{ if($expected>0) printf \"%.1f\", 100.0*$captured/$expected; else print \"0\" }")

  verdict=OK
  if awk "BEGIN{exit !($captured < 0.98*$expected)}"; then verdict=SAMPLE-LOSS; fi
  if [ "$drops" -gt 0 ]; then verdict=DROPS; fi

  printf "%-9d %-12d %-12d %-7s %-7s %s\n" "$fs" "$expected" "$captured" "$drops" "$cappct" "$verdict"

  [ "$KEEP" = "1" ] || rm -f "$file"
done

echo
echo "Legend: OK = A8 kept up | SAMPLE-LOSS = PRU state-ring overflow (A8 drain"
echo "cadence too slow for fs) | DROPS = log-ring overflow (writer/disk too slow)."
