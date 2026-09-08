#!/usr/bin/env python3
"""openWearable scope — binary telemetry listener (data plane only, no rendering).

This is the host-side receiver for the framed-binary UDP telemetry described in
notes/scope-architecture.md. It is deliberately frontend-agnostic: it owns the
socket, the schema, and a ring buffer, and exposes snapshots. A matplotlib view,
a uPlot/WebSocket hub, or a bespoke indicator all sit on top as clients.

Design (the deltas vs. the old gui/pyplotter scripts and deep-spine's scope):
  * Network I/O runs on its OWN thread into a pre-allocated ring -- never inside a
    render/animation callback. That decoupling is what stops the freezes.
  * Payloads are binary and read with a single np.frombuffer into a structured
    dtype -- no ASCII float() parsing, so the whole ValueError-spam class is gone.
  * The dtype is built from a self-describing '#fields:' schema frame sent on the
    wire -- no hand-synced column maps (kills the positional off-by-one bugs).
  * Every data frame carries a monotonic seq -> drops/gaps are COUNTED and visible
    in stats(), never a silent stall.
  * Malformed / short / unknown-schema frames are counted and dropped, never raised.

Wire format (little-endian; see notes/scope-architecture.md):

  header (12 bytes, naturally packed):
    magic[2]='OW'  version:u8  kind:u8  schema_id:u32  seq:u32
  DATA   frame (kind=0): header + record bytes (FormatLogRecord layout)
  SCHEMA frame (kind=1): header + ASCII "#fields: name:type,...\\n#record_bytes: N\\n"

CLI:
  python gui/scope.py --port 1500            # listen + print stats/tail (replaces udplistener.py)
  python gui/scope.py --selftest             # loopback sender+listener, no board needed
"""

import argparse
import socket
import struct
import threading
import time
import zlib
from collections import deque

import numpy as np

# ---------------------------------------------------------------------------
# Wire format
# ---------------------------------------------------------------------------
MAGIC = b"OW"
VERSION = 1
KIND_DATA = 0
KIND_SCHEMA = 1

# magic(2s) version(B) kind(B) schema_id(I) seq(I) -> 12 bytes, no padding under '<'
_HEADER = struct.Struct("<2sBBII")
HEADER_SIZE = _HEADER.size  # 12

# schema type token -> (numpy dtype string, struct pack char, is_fix16)
# fix16 travels as a raw Q16.16 int32 and is decoded to float on snapshot.
_TYPE_MAP = {
    "u8":    ("<u1", "<B", False),
    "i8":    ("<i1", "<b", False),
    "u16":   ("<u2", "<H", False),
    "i16":   ("<i2", "<h", False),
    "u32":   ("<u4", "<I", False),
    "i32":   ("<i4", "<i", False),
    "u64":   ("<u8", "<Q", False),
    "i64":   ("<i8", "<q", False),
    "f32":   ("<f4", "<f", False),
    "f64":   ("<f8", "<d", False),
    "fix16": ("<i4", "<i", True),   # raw int32 on the wire; /65536.0 on decode
}

FIX16_ONE = 65536.0


def parse_fields(fields_text):
    """Parse a '#fields: name:type,...' block into (np.dtype, fix16_names, record_bytes).

    Accepts the exact string FormatLogSchema() emits (leading/trailing lines and the
    '#record_bytes:' line are tolerated). Raises ValueError on an unknown type token.
    """
    fields_line = None
    for line in fields_text.replace("\x00", "").splitlines():
        line = line.strip()
        if line.startswith("#fields:"):
            fields_line = line[len("#fields:"):].strip()
            break
    if fields_line is None:
        raise ValueError("no '#fields:' line in schema frame")

    names, formats, fix16_names = [], [], set()
    for tok in fields_line.split(","):
        tok = tok.strip()
        if not tok:
            continue
        name, _, typ = tok.partition(":")
        name, typ = name.strip(), typ.strip()
        if typ not in _TYPE_MAP:
            raise ValueError(f"unknown field type '{typ}' for '{name}'")
        np_str, _pack, is_fix16 = _TYPE_MAP[typ]
        names.append(name)
        formats.append(np_str)
        if is_fix16:
            fix16_names.add(name)
    if not names:
        raise ValueError("empty '#fields:' list")

    dtype = np.dtype({"names": names, "formats": formats})  # packed, little-endian
    return dtype, fix16_names, dtype.itemsize


def schema_id_of(fields_text):
    """Stable 32-bit id for a schema (crc32 of the normalized '#fields:' line)."""
    dtype, _fix, _n = parse_fields(fields_text)
    canon = ",".join(dtype.names).encode()
    return zlib.crc32(canon) & 0xFFFFFFFF


# ---------------------------------------------------------------------------
# Listener
# ---------------------------------------------------------------------------
class ScopeListener:
    """Threaded binary-UDP telemetry receiver with a ring buffer.

    Start it, then poll snapshot()/latest()/stats() from any thread.
    """

    def __init__(self, port=1500, host="", capacity=2000, recv_bufsize=65536):
        self.port = port
        self.host = host
        self.capacity = int(capacity)
        self.recv_bufsize = recv_bufsize

        self._sock = None
        self._thread = None
        self._stop = threading.Event()
        self._lock = threading.Lock()

        # schema / ring (rebuilt when the active schema_id changes)
        self._schema_id = None
        self._dtype = None
        self._fix16 = set()
        self._ring = None
        self._write = 0          # monotonic write count
        self._filled = 0         # min(_write, capacity)

        # stats
        self._last_seq = None
        self._dropped = 0        # packets lost (seq gaps)
        self._reordered = 0
        self._bad_frames = 0     # malformed / wrong-size / unknown-schema
        self._n_data = 0
        self._n_schema = 0
        self._t_start = None
        self._recent = deque()   # monotonic recv times, trimmed to _WIN seconds
        self._WIN = 2.0

    # -- lifecycle ----------------------------------------------------------
    def start(self):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        # a generous receive buffer so bursts queue in the kernel, not get dropped
        try:
            self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1 << 20)
        except OSError:
            pass
        self._sock.bind((self.host, self.port))
        self._sock.settimeout(0.5)  # blocking w/ timeout so we can honor stop()
        self._t_start = time.monotonic()
        self._thread = threading.Thread(target=self._run, name="scope-rx", daemon=True)
        self._thread.start()
        return self

    def stop(self):
        self._stop.set()
        if self._thread is not None:
            self._thread.join(timeout=2.0)
        if self._sock is not None:
            self._sock.close()

    def __enter__(self):
        return self.start()

    def __exit__(self, *exc):
        self.stop()

    # -- receive loop -------------------------------------------------------
    def _run(self):
        while not self._stop.is_set():
            try:
                pkt, _addr = self._sock.recvfrom(self.recv_bufsize)
            except socket.timeout:
                continue
            except OSError:
                break
            if len(pkt) < HEADER_SIZE:
                self._bad_frames += 1
                continue
            magic, ver, kind, schema_id, seq = _HEADER.unpack_from(pkt, 0)
            if magic != MAGIC or ver != VERSION:
                self._bad_frames += 1
                continue
            payload = pkt[HEADER_SIZE:]
            if kind == KIND_SCHEMA:
                self._handle_schema(schema_id, payload)
            elif kind == KIND_DATA:
                self._handle_data(schema_id, seq, payload)
            else:
                self._bad_frames += 1

    def _handle_schema(self, schema_id, payload):
        try:
            dtype, fix16, _rec = parse_fields(payload.decode("ascii", "ignore"))
        except ValueError:
            self._bad_frames += 1
            return
        with self._lock:
            self._n_schema += 1
            if schema_id == self._schema_id:
                return  # heartbeat, nothing changed
            # new/changed schema -> rebuild ring; old records are incompatible
            self._schema_id = schema_id
            self._dtype = dtype
            self._fix16 = fix16
            self._ring = np.zeros(self.capacity, dtype=dtype)
            self._write = 0
            self._filled = 0
            self._last_seq = None

    def _handle_data(self, schema_id, seq, payload):
        with self._lock:
            if self._dtype is None:
                return  # no schema yet; expected at startup, not an error
            if schema_id != self._schema_id:
                self._bad_frames += 1  # data for a schema we haven't been told about
                return
            if len(payload) != self._dtype.itemsize:
                self._bad_frames += 1
                return

            # seq accounting (wrap-safe via int32 delta)
            if self._last_seq is not None:
                delta = int(np.int32(np.uint32(seq) - np.uint32(self._last_seq)))
                if delta <= 0:
                    self._reordered += 1
                    return  # keep the ring monotonic; drop stale/dupe
                if delta > 1:
                    self._dropped += delta - 1
            self._last_seq = seq

            rec = np.frombuffer(payload, dtype=self._dtype, count=1)[0]
            self._ring[self._write % self.capacity] = rec
            self._write += 1
            self._filled = min(self._write, self.capacity)
            self._n_data += 1

            now = time.monotonic()
            self._recent.append(now)
            cutoff = now - self._WIN
            while self._recent and self._recent[0] < cutoff:
                self._recent.popleft()

    # -- consumer API -------------------------------------------------------
    def snapshot(self, n=None):
        """Return the last n records (chronological) as {field_name: ndarray}.

        fix16 fields are decoded to float64. Non-fix16 fields keep their natural
        numpy dtype. Returns {} until a schema + at least one record have arrived.
        """
        with self._lock:
            if self._dtype is None or self._filled == 0:
                return {}
            m = self._filled if n is None else min(int(n), self._filled)
            start = self._write - m
            idx = np.arange(start, self._write) % self.capacity
            block = self._ring[idx].copy()
            fix16 = self._fix16
        out = {}
        for name in block.dtype.names:
            col = block[name]
            out[name] = col.astype(np.float64) / FIX16_ONE if name in fix16 else col
        return out

    def snapshot_since(self, cursor):
        """Incremental read for streaming consumers (the web hub).

        Returns (new_cursor, cols) where cols is {field: float64 ndarray} for the
        records written since `cursor` (a value previously returned here, or None
        for "give me the current window"). If a consumer falls behind past the ring
        capacity, it silently resyncs to the oldest still-held sample. All fields
        are float64 (fix16 decoded) for uniform JSON serialization.
        """
        with self._lock:
            if self._dtype is None:
                return cursor, {}
            w = self._write
            if cursor is None or cursor > w or (w - cursor) > self._filled:
                cursor = w - self._filled          # first call / fell behind -> resync
            m = w - cursor
            if m <= 0:
                return w, {}
            idx = np.arange(cursor, w) % self.capacity
            block = self._ring[idx].copy()
            fix16 = self._fix16
        cols = {}
        for name in block.dtype.names:
            col = block[name].astype(np.float64)
            cols[name] = col / FIX16_ONE if name in fix16 else col
        return w, cols

    def latest(self):
        """Newest single record as {field_name: python scalar}, or {} if none yet."""
        with self._lock:
            if self._dtype is None or self._filled == 0:
                return {}
            rec = self._ring[(self._write - 1) % self.capacity].copy()
            fix16 = self._fix16
        out = {}
        for name in rec.dtype.names:
            v = rec[name]
            out[name] = float(v) / FIX16_ONE if name in fix16 else v.item()
        return out

    def stats(self):
        with self._lock:
            elapsed = (time.monotonic() - self._t_start) if self._t_start else 0.0
            win_fps = len(self._recent) / self._WIN if self._recent else 0.0
            return {
                "schema_id": self._schema_id,
                "fields": list(self._dtype.names) if self._dtype is not None else [],
                "record_bytes": self._dtype.itemsize if self._dtype is not None else 0,
                "data": self._n_data,
                "schema": self._n_schema,
                "dropped": self._dropped,
                "reordered": self._reordered,
                "bad_frames": self._bad_frames,
                "fill": self._filled,
                "capacity": self.capacity,
                "fps": round(win_fps, 1),
                "avg_fps": round(self._n_data / elapsed, 1) if elapsed > 0 else 0.0,
            }


# ---------------------------------------------------------------------------
# Sender (bench signal source / loopback test — mirrors the C publisher's frames)
# ---------------------------------------------------------------------------
class ScopeSender:
    """Builds and sends framed schema/data packets. For tests and host-side replay."""

    def __init__(self, fields, port=1500, host="127.0.0.1"):
        # fields: list of (name, type_token)
        self.fields = fields
        self.addr = (host, port)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        parts, packers, widths = [], [], 0
        for name, typ in fields:
            if typ not in _TYPE_MAP:
                raise ValueError(f"unknown type '{typ}'")
            np_str, pack, is_fix16 = _TYPE_MAP[typ]
            parts.append(f"{name}:{typ}")
            packers.append((struct.Struct(pack), is_fix16))
            widths += np.dtype(np_str).itemsize
        self.fields_line = "#fields: " + ",".join(parts)
        self._schema_text = f"\n{self.fields_line}\n#record_bytes: {widths}\n"
        self.schema_id = schema_id_of(self._schema_text)
        self._packers = packers
        self._seq = 0

    def _frame(self, kind, seq, payload):
        return _HEADER.pack(MAGIC, VERSION, kind, self.schema_id, seq) + payload

    def send_schema(self):
        self.sock.sendto(
            self._frame(KIND_SCHEMA, 0, self._schema_text.encode("ascii")), self.addr
        )

    def send(self, values):
        buf = bytearray()
        for (packer, is_fix16), v in zip(self._packers, values):
            if is_fix16:
                buf += packer.pack(int(round(v * FIX16_ONE)))
            elif packer.format in ("<f", "<d"):
                buf += packer.pack(float(v))
            else:
                buf += packer.pack(int(v))
        self.sock.sendto(self._frame(KIND_DATA, self._seq, bytes(buf)), self.addr)
        self._seq = (self._seq + 1) & 0xFFFFFFFF


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
def _selftest(port, rate, seconds):
    """Loopback: a sender emits schema + sine data; the listener verifies decode."""
    fields = [("frame", "u32"), ("t", "f32"), ("p1m", "fix16"),
              ("p1d", "fix16"), ("u1", "i32")]
    sender = ScopeSender(fields, port=port, host="127.0.0.1")

    listener = ScopeListener(port=port, host="127.0.0.1").start()
    time.sleep(0.2)

    stop = threading.Event()

    def pump():
        i = 0
        dt = 1.0 / rate
        sender.send_schema()
        next_t = time.monotonic()
        while not stop.is_set():
            t = i * dt
            sender.send([i, t, 2.0 + np.sin(2 * np.pi * 0.5 * t),
                         2.0, 1 if (i // 50) % 2 else -1])
            if i % 200 == 0:
                sender.send_schema()  # heartbeat
            i += 1
            next_t += dt
            time.sleep(max(0.0, next_t - time.monotonic()))

    tx = threading.Thread(target=pump, daemon=True)
    tx.start()

    t_end = time.monotonic() + seconds
    try:
        while time.monotonic() < t_end:
            time.sleep(1.0)
            s = listener.stats()
            snap = listener.snapshot(5)
            tail = {k: (round(float(v[-1]), 4) if len(v) else None)
                    for k, v in snap.items()}
            print(f"[selftest] fps={s['fps']} data={s['data']} drop={s['dropped']} "
                  f"reord={s['reordered']} bad={s['bad_frames']} fill={s['fill']} "
                  f"| tail={tail}")
    finally:
        stop.set()
        listener.stop()

    s = listener.stats()
    ok = (s["data"] > 0 and s["bad_frames"] == 0 and s["dropped"] == 0
          and s["fields"] == [f[0] for f in fields])
    print(f"[selftest] {'PASS' if ok else 'FAIL'} -> {s}")
    return 0 if ok else 1


def _listen(port, host, seconds, period=1.0, col=11):
    """Plain listen: stream the latest values as aligned columns (replaces udplistener.py).

    Header (field names) is reprinted every 20 rows so it stays visible; a compact
    stats line prints every 10 rows.
    """
    listener = ScopeListener(port=port, host=host).start()
    print(f"listening for openWearable binary telemetry on udp://{host or '0.0.0.0'}:{port} ...")
    t_end = None if seconds <= 0 else time.monotonic() + seconds
    fields, rows = [], 0
    try:
        while t_end is None or time.monotonic() < t_end:
            time.sleep(period)
            s = listener.stats()
            if not s["fields"]:
                print("  (waiting for schema frame...)")
                continue
            if s["fields"] != fields:              # (re)print header on schema change
                fields, rows = s["fields"], 0
            snap = listener.snapshot(1)
            if not snap:
                continue
            if rows % 20 == 0:
                print("".join(f"{n:>{col}.{col}}" for n in fields))
            vals = [float(snap[n][-1]) if len(snap[n]) else float("nan") for n in fields]
            print("".join(f"{v:>{col}.4g}" for v in vals))
            rows += 1
            if rows % 10 == 0:
                print(f"  [fps={s['fps']} drop={s['dropped']} reord={s['reordered']} "
                      f"bad={s['bad_frames']} fill={s['fill']}/{s['capacity']}]")
    except KeyboardInterrupt:
        pass
    finally:
        listener.stop()
    return 0


def main():
    ap = argparse.ArgumentParser(description="openWearable binary telemetry scope listener")
    ap.add_argument("-p", "--port", type=int, default=1500)
    ap.add_argument("-H", "--host", type=str, default="", help="bind address (default all)")
    ap.add_argument("-t", "--seconds", type=float, default=0.0,
                    help="run for N seconds then exit (0 = until Ctrl-C)")
    ap.add_argument("--selftest", action="store_true",
                    help="loopback sender+listener, no board required")
    ap.add_argument("--rate", type=float, default=200.0, help="selftest send rate (Hz)")
    args = ap.parse_args()

    if args.selftest:
        raise SystemExit(_selftest(args.port, args.rate,
                                   args.seconds if args.seconds > 0 else 5.0))
    raise SystemExit(_listen(args.port, args.host, args.seconds))


if __name__ == "__main__":
    main()
