"""Dashboard primitives — the composition layer apps hand-roll against.

A per-app dashboard lives in `apps/<app>/gui/dashboard.py` and exposes `build()`
returning a `Dashboard`. It references field names the app already declares in its
`format.c` schema (single source of truth) and only arranges them. If an app has no
dashboard, `auto_dashboard()` builds a one-strip-per-field fallback from the
self-describing stream, so every app is plottable out of the box.

Controls (Toggle/Slider) are Phase 2b (command channel) and intentionally omitted here.
"""

import importlib.util
from pathlib import Path

# Fields commonly used as the x-axis (time base). First match wins for auto-layout.
_X_CANDIDATES = ("frame", "t", "time", "tick", "sample")


class TimePanel:
    """One time-series strip: a titled panel plotting `fields` against the x base."""

    def __init__(self, title, fields, y=None, step=False):
        self.title = title
        self.fields = list(fields)
        self.y = y            # y-axis label (e.g. "kPa"), optional
        self.step = step      # step interpolation (for discrete signals like valve cmds)

    def to_dict(self):
        return {"title": self.title, "fields": self.fields,
                "y": self.y, "step": self.step}


class Control:
    """A control widget bound to a board command (see cmd.c CmdApply).

    kind: "slider" | "toggle". `cmd` is the command name the board maps; `echo` is
    the telemetry field that confirms the applied value (so the widget reflects real
    board state). All commands are gated by the console's single ARM switch.
    """

    def __init__(self, kind, label, cmd, echo=None, lo=0.0, hi=1.0, step=0.01):
        self.kind = kind
        self.label = label
        self.cmd = cmd
        self.echo = echo
        self.lo, self.hi, self.step = lo, hi, step

    def to_dict(self):
        return {"kind": self.kind, "label": self.label, "cmd": self.cmd,
                "echo": self.echo, "lo": self.lo, "hi": self.hi, "step": self.step}


def Slider(label, cmd, echo=None, lo=0.0, hi=1.0, step=0.01):
    return Control("slider", label, cmd, echo, lo, hi, step)


def Toggle(label, cmd, echo=None):
    return Control("toggle", label, cmd, echo)


class Dashboard:
    """Panels + controls + an x field. Serializes to the layout the browser renders."""

    def __init__(self, title="openWearable", x=None):
        self.title = title
        self.x = x            # x-axis field name; None -> auto-detect / sample index
        self.panels = []
        self.controls = []

    def add(self, panel):
        self.panels.append(panel)
        return self

    def control(self, c):
        self.controls.append(c)
        return self

    def to_dict(self):
        return {"title": self.title, "x": self.x,
                "panels": [p.to_dict() for p in self.panels],
                "controls": [c.to_dict() for c in self.controls]}


def pick_x(fields, x=None):
    """Resolve the x field: explicit, else first schema field matching a known name."""
    if x is not None:
        return x
    for cand in _X_CANDIDATES:
        if cand in fields:
            return cand
    return None  # browser falls back to a running sample index


def auto_dashboard(fields, title="openWearable (auto)", x=None):
    """Fallback layout: one strip per non-x field, in schema order."""
    x = pick_x(fields, x)
    d = Dashboard(title=title, x=x)
    for f in fields:
        if f == x:
            continue
        d.add(TimePanel(f, [f]))
    return d


def load_app_dashboard(app_name, repo_root):
    """Import apps/<app>/gui/dashboard.py by path and call build(); None if absent.

    Loaded by file path (not as a package) so apps need not be importable modules.
    """
    path = Path(repo_root) / "apps" / app_name / "gui" / "dashboard.py"
    if not path.exists():
        return None
    spec = importlib.util.spec_from_file_location(f"ow_dashboard_{app_name}", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "build"):
        raise AttributeError(f"{path} defines no build() -> Dashboard")
    return mod.build()
