#!/usr/bin/env python3
"""openWearable web hub (Phase 2a — one-way browser scope).

Bridges the framed-binary UDP telemetry (via gui/scope.py's ScopeListener) to any
number of browsers over WebSocket, and serves a generic uPlot frontend that renders
whatever dashboard the app declares. The board is untouched: it already publishes the
binary stream this hub ingests.

Run:
  python -m gui.webhub.server --app reflex-demo --udp 1500 --http 8080
  python -m gui.webhub.server --selftest            # built-in synthetic source, no board

Then open http://<this-host>:8080 from a laptop or phone on the same network.
"""

import argparse
import asyncio
import threading
import time
from pathlib import Path

import numpy as np
from aiohttp import web

from gui.scope import ScopeListener, ScopeSender
from gui.webhub.dashboard import auto_dashboard, load_app_dashboard

WEB_DIR = Path(__file__).parent / "web"
REPO_ROOT = Path(__file__).resolve().parents[2]


class Hub:
    """Owns the listener + resolved dashboard; each WS connection streams from it."""

    def __init__(self, listener, app_name, fps):
        self.listener = listener
        self.app_name = app_name
        self.fps = fps
        self._dashboard = None  # resolved lazily once the schema is known

    def dashboard(self):
        """Resolve (once) the per-app dashboard, or the auto fallback. None until schema."""
        if self._dashboard is not None:
            return self._dashboard
        fields = self.listener.stats()["fields"]
        if not fields:
            return None
        d = load_app_dashboard(self.app_name, REPO_ROOT) if self.app_name else None
        if d is None:
            d = auto_dashboard(fields)
        self._dashboard = d
        return d


async def ws_handler(request):
    hub = request.app["hub"]
    ws = web.WebSocketResponse(heartbeat=20.0)
    await ws.prepare(request)

    # Wait for the schema (dashboard) before streaming.
    dash = None
    for _ in range(150):  # up to ~30 s
        dash = hub.dashboard()
        if dash is not None:
            break
        await asyncio.sleep(0.2)
    if dash is None:
        await ws.close(message=b"no schema (is the board publishing?)")
        return ws

    await ws.send_json({"type": "init",
                        "dashboard": dash.to_dict(),
                        "fields": hub.listener.stats()["fields"]})

    async def sender():
        cursor = None  # None -> first send is the current window (backlog), then increments
        period = 1.0 / hub.fps
        while not ws.closed:
            await asyncio.sleep(period)
            cursor, cols = hub.listener.snapshot_since(cursor)
            if cols:
                await ws.send_json({"type": "data",
                                    "cols": {k: v.tolist() for k, v in cols.items()}})

    send_task = asyncio.create_task(sender())
    try:
        async for _msg in ws:   # inbound: reserved for Phase 2b commands; ignored in 2a
            pass
    finally:
        send_task.cancel()
    return ws


async def index(request):
    return web.FileResponse(WEB_DIR / "index.html")


def build_app(hub):
    app = web.Application()
    app["hub"] = hub
    app.router.add_get("/", index)
    app.router.add_get("/ws", ws_handler)
    app.router.add_static("/", WEB_DIR)  # app.js, style.css, vendor/*
    return app


def _start_selftest_source(udp_port):
    """A synthetic board: schema + sine/step data to 127.0.0.1:udp_port (no hardware)."""
    fields = [("frame", "u32"), ("t", "f32"), ("p1m", "fix16"),
              ("p1d", "fix16"), ("u1", "i32")]
    snd = ScopeSender(fields, port=udp_port, host="127.0.0.1")

    def pump():
        i, dt = 0, 1.0 / 200
        snd.send_schema()
        nxt = time.monotonic()
        while True:
            t = i * dt
            snd.send([i, t, 2.0 + np.sin(2 * np.pi * 0.5 * t),
                      2.0, 1 if (i // 50) % 2 else -1])
            if i % 200 == 0:
                snd.send_schema()
            i += 1
            nxt += dt
            time.sleep(max(0.0, nxt - time.monotonic()))

    threading.Thread(target=pump, daemon=True, name="selftest-src").start()


def main():
    ap = argparse.ArgumentParser(description="openWearable web hub (browser scope)")
    ap.add_argument("--app", default=None,
                    help="app name -> loads apps/<app>/gui/dashboard.py (else auto-layout)")
    ap.add_argument("--udp", type=int, default=1500, help="telemetry UDP port to listen on")
    ap.add_argument("--http", type=int, default=8080, help="web/WebSocket port to serve")
    ap.add_argument("--fps", type=float, default=50.0, help="push rate to browsers")
    ap.add_argument("--selftest", action="store_true",
                    help="run a built-in synthetic source (no board required)")
    args = ap.parse_args()

    bind = "127.0.0.1" if args.selftest else ""
    listener = ScopeListener(port=args.udp, host=bind).start()
    if args.selftest:
        _start_selftest_source(args.udp)

    hub = Hub(listener, args.app, args.fps)
    app = build_app(hub)
    print(f"web hub: http://0.0.0.0:{args.http}  (udp:{args.udp}, "
          f"app={args.app or 'auto'}, {args.fps:.0f} fps)")
    try:
        web.run_app(app, host="0.0.0.0", port=args.http, print=None)
    finally:
        listener.stop()


if __name__ == "__main__":
    main()
