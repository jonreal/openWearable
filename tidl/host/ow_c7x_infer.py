#!/usr/bin/env python3
"""openWearable A72-side host tool for the fleet-free C7x TIDL mailbox (Path-Y step 2).

Stages a compiled TIDL net + input into DDR_SHARED_MEM via /dev/mem, kicks the C7x, and reads
back the float output. The C7x firmware side is tidl/src/ow_c7x_tidl.c. This proves the
A72<->C7x shared-memory mailbox that becomes the cpuloop transport. Pure stdlib (no numpy) so it
runs on the board's minimal Python.

Run on the board (root):
    python3 ow_c7x_infer.py net.bin io.bin     # input defaults to the step-2 test vector

Mailbox layout MUST match ow_c7x_tidl.c.
"""
import mmap, os, struct, sys, time

BASE   = 0xB8000000
MAPLEN = 0x02200000           # ~34 MB window
READY, DONE, STATUS, NETLEN, IOLEN = 0x00, 0x10, 0x20, 0x30, 0x38
INVOKES = 0x70                # C7x cumulative invoke counter (multi-invoke loop)
DBG = 0x40                    # 10 firmware progress markers (ow_c7x_tidl.c OW_DBG)
NET, IO, INPUT, OUTPUT = 0x00100000, 0x02000000, 0x02100000, 0x02180000
DBG_LABELS = ["enter", "sciclient_rc", "udma_rc", "udma_ptr", "wait_ready",
              "netlen", "pre_create", "create_st", "pre_invoke", "invoke_st"]
N_IN, N_OUT = 16, 8
RDY_MAGIC, DONE_MAGIC = 0xC7C7AA01, 0xC7C7DD01

# fixed test vector + float reference (matches gen_tiny_mlp.py)
X_DEFAULT = [(i / float(N_IN)) - 0.5 for i in range(N_IN)]
REF = [0.0, 0.3083, 0.1019, 0.0454, 0.0, 0.2057, 0.2034, 0.0]


def fmt(v):
    return "[" + ", ".join("%.4f" % x for x in v) + "]"


def main():
    net = open(sys.argv[1], "rb").read()
    io_ = open(sys.argv[2], "rb").read()
    x = list(X_DEFAULT)

    fd = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
    m = mmap.mmap(fd, MAPLEN, mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE, offset=BASE)

    m[DONE:DONE + 4] = struct.pack("<I", 0)
    # Clear ONLY the create/probe markers (written after READY); NOT the OW_DBG boot markers
    # (0x40-0x6C, written at boot before we run) and NOT the log header.
    m[0x80:0x80 + 0x40] = b"\x00" * 0x40              # create + AllocMemRecords probes
    m[0x200:0x200 + 0x800] = b"\x00" * 0x800          # firmware text log
    m[NET:NET + len(net)] = net
    m[IO:IO + len(io_)] = io_
    m[INPUT:INPUT + N_IN * 4] = struct.pack("<%df" % N_IN, *x)
    m[NETLEN:NETLEN + 4] = struct.pack("<I", len(net))
    m[IOLEN:IOLEN + 4] = struct.pack("<I", len(io_))
    m[READY:READY + 4] = struct.pack("<I", RDY_MAGIC)     # kick the C7x
    print("staged net=%dB io=%dB input=%s; waiting for C7x..." % (len(net), len(io_), fmt(x)))

    def dbg():
        vals = struct.unpack("<10I", m[DBG:DBG + 40])
        print("firmware markers:")
        for lbl, v in zip(DBG_LABELS, vals):
            sv = v if v < 0x80000000 else v - 0x100000000
            print("  %-13s = 0x%08x (%d)" % (lbl, v, sv))
        txt = m[0x200:0x200 + 0x800].split(b"\x00", 1)[0].decode("ascii", "replace")
        if txt.strip():
            print("firmware log:\n  " + txt.replace("\n", "\n  ").rstrip())

    t0 = time.time()
    while struct.unpack("<I", m[DONE:DONE + 4])[0] != DONE_MAGIC:
        if time.time() - t0 > 10:
            print("TIMEOUT (no DONE). status=%#x" % struct.unpack("<I", m[STATUS:STATUS + 4])[0])
            dbg()
            return 1
    st = struct.unpack("<i", m[STATUS:STATUS + 4])[0]
    dbg()
    out = list(struct.unpack("<%df" % N_OUT, m[OUTPUT:OUTPUT + N_OUT * 4]))
    ninv = struct.unpack("<I", m[INVOKES:INVOKES + 4])[0]
    err = max(abs(o - r) for o, r in zip(out, REF))
    print("C7x status=%d  invoke#=%d  output=%s" % (st, ninv, fmt(out)))
    print("float ref     =%s" % fmt(REF))
    print("max |abs err| =%.4f  %s" % (err, "PASS" if err < 0.05 else "CHECK (int8 quant?)"))
    m.close(); os.close(fd)
    return 0


if __name__ == "__main__":
    sys.exit(main())
