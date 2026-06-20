# R5F &harr; ICSSG shared-RAM access probe

Gating test for the **unified PRU+R5F memory model**: can the R5F (main domain)
read/write the PRU's **ICSSG shared RAM at `0x0b010000`**? If yes, the R5F can
sample-and-hold off the PRU (one time base via frame-count decimation, one log).
If a firewall blocks it, we fall back to the separate-memory / separate-log model.

## What it does
A core0 R5F firmware that, in a loop, (1) writes a pattern to ICSSG scratch
(`0x0b012000`) and reads it back, and (2) reads the PRU's tick counter
(`param.debug_buff[0]` @ `0x0b01e000`). Results land in the core0 DDR carveout
(`0xa2100400`, always reachable). A firewall block traps to the data-abort
handler, which stamps `0xDEAD` + the fault regs (`DFAR`/`DFSR`) there.

## Run it
```
make                 # build
make install         # stand in for the park fw on core0 (backs it up)
reboot               # probe boots on core0
# bring the ICSSG up (and PRU live data): run the OW app once.
#   NOTE: template -v hangs on the FIRST run after boot -- run it twice.
#   NOTE: never run more than one app instance.
apps/bin/template -v   # starts + enables the PRUs (ICSSG clocked, debug_buff ticking)
./read.py            # read the probe results
make restore && reboot # put the park back
```

## Interpret `read.py`
- `icssg w/rb` shows a climbing `0xC0FFEExx` **and** `DATA ABORT: none`
  &rarr; **R5F can R/W the ICSSG RAM** &rarr; go **unified**.
- `pru dbg` climbing &rarr; the R5F is reading the PRU's *live* data (full path proven).
- `DATA ABORT: YES` (with `DFAR`/`DFSR`) &rarr; **firewall/no access** &rarr; go **separate**.
- `icssg w/rb` static / not matching with no abort &rarr; ICSSG not clocked; make sure a
  PRU app is running, then re-read.
