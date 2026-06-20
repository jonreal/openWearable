#!/usr/bin/env python3
# Read the R5F<->ICSSG probe results from the core0 carveout (0xa2100400).
import mmap, os, struct
fd = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
m = mmap.mmap(fd, 0x1000, offset=0xa2100000)
def rd(o): return struct.unpack_from("<I", m, 0x400 + o)[0]
magic, beat, wb, prudbg, ab, dfar, dfsr = (rd(0), rd(4), rd(8), rd(0xC), rd(0x10), rd(0x14), rd(0x18))
print("probe magic : 0x%08x  (%s)" % (magic, "ALIVE" if magic == 0x5F5F0B10 else "not running"))
print("beat        : %d" % beat)
print("icssg w/rb  : 0x%08x  (expect climbing 0xC0FFEExx => R5F can R/W ICSSG RAM)" % wb)
print("pru dbg     : %d  (climbs if a PRU app is running => R5F reads PRU's live data)" % prudbg)
if ab == 0xDEAD:
    print("DATA ABORT  : YES   DFAR=0x%08x DFSR=0x%08x  => firewall/no access (use SEPARATE model)" % (dfar, dfsr))
else:
    print("DATA ABORT  : none")
