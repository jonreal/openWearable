#!/usr/bin/env python3
import socket, os, errno, fcntl
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# -------- UDP setup --------
UDP_PORT_NO = 1500
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', UDP_PORT_NO))
fcntl.fcntl(sock, fcntl.F_SETFL, os.O_NONBLOCK)

_latest_pkt = None
last_time = None
last_values = []

def _recv_latest():
    global _latest_pkt
    while True:
        try:
            _latest_pkt, _ = sock.recvfrom(512)
        except OSError as e:
            if e.errno == errno.EAGAIN:
                break
            raise

def _pump_packet():
    """Receive+parse newest packet; update last_time/last_values. Returns (t, values) or (None, [])."""
    global last_time, last_values
    _recv_latest()
    if _latest_pkt is None:
        return None, []
    try:
        parts = _latest_pkt.decode("utf-8").replace("\x00", "").strip().split('\t')
        t = float(parts[0]) / 1000.0
        vals = [float(v) for v in parts[1:] if v.strip()]
        last_time, last_values = t, vals
        return t, vals
    except Exception as e:
        print("Error processing packet:", e)
        return None, []

# -------- Figure 1: time series --------
fig_ts, ax_ts = plt.subplots()
x = np.array([])
y_data = []
lines = []

ax_ts.set_xlim(-10, 0)
ax_ts.set_ylim(0, 60)

def init_ts():
    for ln in lines:
        ln.set_data([], [])
    return lines

def animate_ts(_i):
    global x, y_data, lines
    t, vals = _pump_packet()
    if t is None:
        return lines

    # echo to console
    print(f"Time: {t:.3f}, Values: {vals}")

    # (re)build lines when channel count changes
    if len(y_data) != len(vals):
        y_data = [np.array([]) for _ in range(len(vals))]
        ax_ts.clear()
        lines.clear()
        for _ in range(len(vals)):
            ln, = ax_ts.plot([], [], lw=2)
            lines.append(ln)

    # maintain ~300 samples
    x = np.append(x[-299:], t)
    for k in range(len(vals)):
        y_data[k] = np.append(y_data[k][-299:], vals[k])

    # update axes
    if x.size > 1:
        ax_ts.set_xlim(x.min(), x.max())
    if y_data and any(arr.size for arr in y_data):
        ymin = min(arr.min() for arr in y_data if arr.size)
        ymax = max(arr.max() for arr in y_data if arr.size)
        if np.isfinite(ymin) and np.isfinite(ymax) and ymin != ymax:
            pad = 0.1 * (ymax - ymin)
            ax_ts.set_ylim(ymin - pad, ymax + pad)

    # update data
    for k, ln in enumerate(lines):
        ln.set_data(x, y_data[k])

    return lines

# -------- Figure 2: sit/stand indicator --------
fig_ind, ax_ind = plt.subplots(figsize=(8, 5))
ax_ind.axis("off")
ax_ind.set_xlim(0, 1)
ax_ind.set_ylim(0, 1)

SIT_COLOR, STAND_COLOR = "#FF0000", "#00A000"
ALPHA_SOLID, ALPHA_FADE = 1.0, 0.25

txt_stand = ax_ind.text(0.5, 0.68, "Stand", ha="center", va="center",
                        fontsize=160, fontweight="bold", color=STAND_COLOR, alpha=ALPHA_FADE)
txt_sit   = ax_ind.text(0.5, 0.32, "Sit",   ha="center", va="center",
                        fontsize=160, fontweight="bold", color=SIT_COLOR,   alpha=ALPHA_SOLID)  # start Sit solid

def _bit_from_last(v):
    # strict bit semantics: 1 -> stand, else -> sit
    try:
        return 1 if int(round(v)) == 1 else 0
    except Exception:
        return 0

def init_ind():
    # start with Sit solid, Stand faded
    txt_sit.set_alpha(ALPHA_SOLID)
    txt_stand.set_alpha(ALPHA_FADE)
    return (txt_sit, txt_stand)

def animate_ind(_i):
    # use shared state; do NOT read socket here to avoid double-consume
    if last_values:
        state = _bit_from_last(last_values[-1])
        if state == 1:
            txt_stand.set_alpha(ALPHA_SOLID)
            txt_sit.set_alpha(ALPHA_FADE)
        else:
            txt_stand.set_alpha(ALPHA_FADE)
            txt_sit.set_alpha(ALPHA_SOLID)
    else:
        # no data yet: keep Sit solid
        txt_stand.set_alpha(ALPHA_FADE)
        txt_sit.set_alpha(ALPHA_SOLID)
    return (txt_sit, txt_stand)

# Two animations, shared state
anim_ts  = FuncAnimation(fig_ts,  animate_ts, init_func=init_ts,
                         interval=33, blit=False, cache_frame_data=False)
anim_ind = FuncAnimation(fig_ind, animate_ind, init_func=init_ind,
                         interval=33, blit=False, cache_frame_data=False)

plt.show()
