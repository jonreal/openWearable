# openWearable web hub

Browser scope for the framed-binary UDP telemetry. The board publishes; this host-side
hub ingests (via `gui/scope.py`), and serves a uPlot page to any browser on the network —
laptop or phone. See `notes/scope-architecture.md` for the full design.

```
gui/
  scope.py              # ScopeListener — UDP ingest (reused)
  webhub/
    server.py           # aiohttp: ingest + /ws + serve web/
    dashboard.py        # Dashboard / TimePanel primitives + auto-layout
    web/                # generic vanilla + uPlot frontend (renders any dashboard)
apps/<app>/gui/
    dashboard.py        # thin per-app layout (build() -> Dashboard)
```

## Run

```sh
# synthetic source, no board needed:
python -m gui.webhub.server --selftest

# against a live board publishing to this host (board: app -u <this-host>):
python -m gui.webhub.server --app reflex-demo --udp 1500 --http 8080
```

Open `http://<this-host>:8080` (find the host IP with `ipconfig getifaddr en0`). Apps with
no `apps/<app>/gui/dashboard.py` get a one-strip-per-field auto layout from the stream.

## Add a dashboard for an app

Create `apps/<app>/gui/dashboard.py` with a `build()` returning a `Dashboard`, referencing
the field names from that app's `format.c` schema. See `apps/reflex-demo/gui/dashboard.py`.

Phase 2b (command channel: browser controls -> board) is not built yet.
