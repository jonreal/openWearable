"""openWearable web hub — shared host-side scope/telemetry stack.

`gui/` is the host "library/": reusable machinery lives here; each app hand-rolls a
thin instance in `apps/<app>/gui/dashboard.py` using these primitives. See
notes/scope-architecture.md.
"""

from gui.webhub.dashboard import (
    Dashboard, TimePanel, Slider, Toggle, auto_dashboard, load_app_dashboard,
)

__all__ = ["Dashboard", "TimePanel", "Slider", "Toggle",
           "auto_dashboard", "load_app_dashboard"]
