#!/usr/bin/env python3
# ow_dru_dump.py - A72-side /dev/mem reader for the C7x MMA DRU registers.
# Dumps global DRU config + per-channel CHRT/CHNRT/CHATOMIC for a register diff
# between a working (stock fleet) C7x firmware and our minimal TIDLRT firmware.
import mmap, os, struct, sys, time

DRU_BASE   = 0x6D000000
MAP_LEN    = 0x00100000            # covers CFG..CAUSE (0xE0000)
NUM_CH     = 32

# offsets relative to DRU_BASE
OFF_CFG        = 0x000000          # global DRU config block
OFF_SET        = 0x004000
OFF_QCFG       = 0x008000          # queue cfg,   stride 8, 5 queues
OFF_QSTATUS    = 0x008040          # queue status,stride 8, 5 queues
OFF_CHNRT      = 0x040000          # stride 0x100: CFG@0x00 CHOES0@0x20 CHST_SCHED@0x60
OFF_CHRT       = 0x060000          # stride 0x100: CTL@0x00 SWTRIG@0x08 STATUS_DET@0x10 STATUS_CNT@0x18
OFF_CHATOMIC   = 0x080000          # stride 0x40 debug image of TR
OFF_CAUSE      = 0x0E0000

def main():
    label = sys.argv[1] if len(sys.argv) > 1 else "snapshot"
    fd = os.open("/dev/mem", os.O_RDONLY | os.O_SYNC)
    mm = mmap.mmap(fd, MAP_LEN, mmap.MAP_SHARED, mmap.PROT_READ, offset=DRU_BASE)
    def r32(off):
        return struct.unpack("<I", mm[off:off+4])[0]
    def r64(off):
        return struct.unpack("<Q", mm[off:off+8])[0]

    print("=== DRU REGISTER DUMP [%s] @ %s ===" % (label, time.strftime("%H:%M:%S")))
    print("-- global CFG (0x6D000000) --")
    for i in range(16):
        v = r32(OFF_CFG + i*4)
        if v: print("  CFG[%2d] @+0x%04x = 0x%08x" % (i, OFF_CFG+i*4, v))
    print("-- SET (0x6D004000) --")
    for i in range(8):
        v = r32(OFF_SET + i*4)
        if v: print("  SET[%2d] @+0x%04x = 0x%08x" % (i, OFF_SET+i*4, v))
    print("-- QUEUE cfg/status (5 queues) --")
    for q in range(5):
        c = r32(OFF_QCFG + q*8); s = r32(OFF_QSTATUS + q*8)
        print("  Q%d  CFG=0x%08x  STATUS=0x%08x" % (q, c, s))
    print("  CAUSE = 0x%08x" % r32(OFF_CAUSE))

    print("-- active channels (non-default CHRT/CHNRT) --")
    active = []
    for ch in range(NUM_CH):
        ctl   = r32(OFF_CHRT + ch*0x100 + 0x00)
        swt   = r32(OFF_CHRT + ch*0x100 + 0x08)
        sdet  = r32(OFF_CHRT + ch*0x100 + 0x10)
        scnt  = r64(OFF_CHRT + ch*0x100 + 0x18)
        ncfg  = r32(OFF_CHNRT + ch*0x100 + 0x00)
        choe0 = r32(OFF_CHNRT + ch*0x100 + 0x20)
        sched = r32(OFF_CHNRT + ch*0x100 + 0x60)
        # default idle channel: CTL=0x80000000 (enabled-default) or 0, CHOES0=0xffff, rest 0
        nondefault = not (ctl in (0x0, 0x80000000) and swt==0 and sdet==0 and scnt==0
                          and ncfg==0 and choe0==0xffff and sched==0)
        if nondefault or scnt != 0:
            active.append(ch)
            print("  CH%2d CTL=0x%08x SWTRIG=0x%08x STATUS_DET=0x%08x STATUS_CNT=0x%016x" % (ch,ctl,swt,sdet,scnt))
            print("       CHNRT.CFG=0x%08x CHOES0=0x%08x CHST_SCHED=0x%08x" % (ncfg,choe0,sched))
    if not active:
        print("  (none non-default)")

    print("-- CHATOMIC debug image for active channels --")
    for ch in active:
        words = [r32(OFF_CHATOMIC + ch*0x40 + i*4) for i in range(16)]
        for base in (0x00,0x10,0x20,0x30):
            row = " ".join("%08x" % words[(base//4)+k] for k in range(4))
            print("  CH%2d +0x%02x: %s" % (ch, base, row))
    mm.close(); os.close(fd)

if __name__ == "__main__":
    main()

def watch():
    # poll all 32 CHRT.CTL for ENABLE(bit31); snapshot CHATOMIC of any live channel
    import mmap as _m, os as _o, struct as _s, time as _t
    fd=_o.open("/dev/mem",_o.O_RDONLY|_o.O_SYNC)
    mm=_m.mmap(fd,MAP_LEN,_m.MAP_SHARED,_m.PROT_READ,offset=DRU_BASE)
    def r32(o): return _s.unpack("<I",mm[o:o+4])[0]
    def r64(o): return _s.unpack("<Q",mm[o:o+8])[0]
    seen={}
    t0=_t.time(); n=0
    while _t.time()-t0 < 12.0:
        n+=1
        for ch in range(NUM_CH):
            ctl=r32(OFF_CHRT+ch*0x100+0x00)
            if ctl & 0x80000000:
                scnt=r64(OFF_CHRT+ch*0x100+0x18)
                w=[r32(OFF_CHATOMIC+ch*0x40+i*4) for i in range(16)]
                key=(ch,w[2],w[10] if len(w)>10 else 0)
                if key not in seen:
                    seen[key]=(ctl,scnt,list(w))
    mm.close(); _o.close(fd)
    print("=== WATCH: distinct live (CTL.ENABLE) channel/TR snapshots: %d (polls=%d) ===" % (len(seen),n))
    for (ch,_a,_b),(ctl,scnt,w) in sorted(seen.items()):
        print("CH%2d CTL=0x%08x STATUS_CNT=0x%016x" % (ch,ctl,scnt))
        for base in (0,4,8,12):
            print("     +0x%02x: %08x %08x %08x %08x" % (base*4, w[base],w[base+1],w[base+2],w[base+3]))

if __name__ == "__main__" and len(sys.argv)>1 and sys.argv[1]=="watch":
    watch(); sys.exit(0)
