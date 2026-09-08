"""reflex-demo dashboard — thin per-app instance (host-side).

Hand-rolled layout over the shared gui/ primitives. Field names come from this app's
format.c schema (the single source of truth); this file only arranges them.
Run:  python -m gui.webhub.server --app reflex-demo --udp 1500 --http 8080
"""

from gui.webhub import Dashboard, TimePanel, Slider, Toggle


def build():
    d = Dashboard(title="reflex-demo", x="frame")
    d.add(TimePanel("PAM 1 pressure", ["p1m_raw", "p1m", "p1d"], y="kPa"))
    d.add(TimePanel("PAM 2 pressure", ["p2m_raw", "p2m", "p2d"], y="kPa"))
    d.add(TimePanel("Valves", ["u1", "u2"], step=True))
    d.add(TimePanel("Reflex trigger", ["trigger"]))
    d.add(TimePanel("Reservoir", ["p_res"], y="kPa"))
    d.add(TimePanel("Pots (EP / stiffness)", ["pot1", "pot2"]))

    # Controls -> board command channel (all gated by the console ARM switch).
    # `echo` fields are the telemetry values that confirm what the board applied.
    d.control(Slider("Pmax (kPa)",      cmd="Pmax",      echo="pmax", lo=0, hi=40, step=1))
    d.control(Slider("dP (jump)",       cmd="dP",        echo="dp",   lo=0, hi=10, step=0.5))
    d.control(Slider("reflex threshold", cmd="threshold", echo="thr",  lo=0, hi=0.2, step=0.005))
    d.control(Toggle("Reflex",          cmd="reflex",    echo="reflex"))
    return d
