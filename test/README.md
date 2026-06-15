# openWearable tests

Tests live here, mirroring `apps/` but kept separate from it:

- Each test builds to **`test/bin/`** (not `apps/bin/`), so test binaries never
  land on `$PATH` next to real apps.
- Test firmware uses a test-specific name (`am335x-pruN-<test>-fw`), so it never
  clobbers an app's installed firmware.

## Test form: full-OW, with a host-unit escape hatch

The default form is a **full-OW integration test**: a test is a normal openWearable
app that brings up the *entire* stack (PRU + A8 + the library it exercises) and must
**tear down cleanly** every run. On this platform the cross-core integration *is*
the product (deterministic hand-off across cores), so exercising the real assembled
system catches what a mocked unit test cannot.

To stay DRY, a test reuses the `apps/template` hooks it doesn't need to change and
supplies only the file it does. `logbench` is the model: its Makefile pulls
`state.h`, `pruloop.c`, `cpuloop.c`, `format.c`, and the `PRU*.cmd` linker scripts
from `apps/template/` via `vpath`, and provides only a headless `uiloop.c`. So
`logbench == the template app with the interactive menu swapped for a benchmark
driver`.

Reserve plain **host-compiled unit tests** (no board, no PRU) for pure-host modules
that have no hardware dependency — e.g. the `utils/logdecode` decoder or `fix16`
math. Add those under `test/<module>/` with their own host Makefile when needed.

## logbench — logging throughput benchmark

Finds the sample-rate (`fs`) at which the A8 can no longer consume PRU state fast
enough. It ramps `fs` and, at each step, runs the whole stack logging for a fixed
duration, then reports two independent "can't keep up" signals:

| signal        | meaning                                                              |
|---------------|---------------------------------------------------------------------|
| `DROPS`       | log-ring overflow — the writer thread / disk couldn't drain the 4 MB RAM ring |
| `SAMPLE-LOSS` | PRU state-ring overflow — the fixed ~200 Hz A8 drain fell behind `fs` upstream of the log ring |

The limit is the highest `fs` with no drops and ~100% capture.

**Architecture note.** The A8 logging producer is a fixed-rate `SIGALRM` timer
(~200 Hz); raising `fs` enlarges the *batch* the A8 drains from the PRU state-ring
each tick, it does not speed the tick up. With `STATE_BUFF_LEN = 96` that gives an
A8-cadence ceiling around **96 / 0.005 s ≈ 19.2 kHz** before the PRU ring overflows —
which at 16 B/record is only ~300 KB/s, far below any eMMC limit. So on this board
the expected wall is `SAMPLE-LOSS` near ~19 kHz, not `DROPS`. Levers to push it
higher: a deeper `STATE_BUFF_LEN`, a faster A8 timer, or DMA off the state-ring.

### Run (on the board)

```sh
# builds libow_pru + logbench, then sweeps the default fs ladder to eMMC
test/logbench/run_logbench.sh

# isolate the consumer from storage: log to a tmpfs (RAM) sink
SINK=/dev/shm test/logbench/run_logbench.sh

# custom sweep / longer runs / keep the captured files
FREQS="1000 10000 19000 19500 20000" SECS=15 KEEP=1 test/logbench/run_logbench.sh
```

Each captured file is a normal openWearable binary log; decode it with
`utils/logdecode/` to inspect the `time` column for gaps.
