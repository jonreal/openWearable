import socket
import sys
import numpy as np
import fcntl, os
import errno
from matplotlib import pyplot as plt
from matplotlib import animation

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 1500
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.bind(('', UDP_PORT_NO))
fcntl.fcntl(serverSock, fcntl.F_SETFL, os.O_NONBLOCK)

# Initialize empty buffers
x = np.array([])
y_data = []
latest_packet = None  # Store the latest packet

## Setup figure and subplots
#fig, axs = plt.subplots(2, 2, figsize=(10, 6))
#top_left_ax = axs[0, 0]
#top_right_ax = axs[0, 1]
#bottom_left_ax = axs[1, 0]
#bottom_right_ax = axs[1, 1]
#trigger_ax = top_left_ax.twinx()

fig, axs = plt.subplots(
    2, 2, figsize=(10, 6),
    gridspec_kw={'height_ratios': [3, 1]},   # top 3x taller than bottom
    constrained_layout=True                  # handles spacing; drop tight_layout()
)
top_left_ax, top_right_ax = axs[0]
bottom_left_ax, bottom_right_ax = axs[1]
trigger_ax = top_left_ax.twinx()

bottom_left_ax.sharex(top_left_ax)
bottom_right_ax.sharex(top_right_ax)

lines = []
axes_initialized = False

last_seen_t = float('-inf')


def init():
    global lines
    for line in lines:
        line.set_data([], [])
    return lines


def receive_latest_packet():
    global latest_packet
    newest = None
    newest_t = -1.0
    while True:
        try:
            pkt, _ = serverSock.recvfrom(2048)  # bigger than worst-case line
            parts = pkt.decode("utf-8", "ignore").replace("\x00","").strip().split('\t')
            if len(parts) >= 2:
                t = float(parts[0]) / 1000.0
                if t > newest_t:
                    newest_t, newest = t, pkt
        except (BlockingIOError, OSError) as e:
            if isinstance(e, BlockingIOError) or getattr(e, "errno", None) in (errno.EAGAIN, errno.EWOULDBLOCK):
                break
            raise
    if newest is not None:
        latest_packet = newest

def animate(i):
    global x, y_data, lines, latest_packet, axes_initialized, last_seen_t

    receive_latest_packet()
    if latest_packet is None:
        return lines

    try:
        val = latest_packet.decode("utf-8").replace("\x00", "").strip().split('\t')
        if len(val) < 19:
            return lines

        time = float(val[0]) / 1000.0

        if time <= last_seen_t:
            return lines  # drop out-of-order/duplicate
        last_seen_t = time


        values = [float(v) for v in val[1:] if v.strip()]
        print(f"Time: {time:.3f}, Values: {values}")

        if len(y_data) != len(values):
            y_data = [np.array([]) for _ in range(len(values))]
            lines.clear()
            #top_left_ax.clear()
            #top_right_ax.clear()
            #bottom_left_ax.clear()
            #bottom_right_ax.clear()

            # Plot p1 (1), p1d (3), p1m (2), and trigger (17) in top-left
            lines += [top_left_ax.plot([], [], label='p1m_raw')[0],
                      top_left_ax.plot([], [], label='p1m')[0],
                      top_left_ax.plot([], [], label='p1d')[0],
                      top_left_ax.plot([], [], label='p2m_raw')[0],
                      top_left_ax.plot([], [], label='p2m')[0],
                      top_left_ax.plot([], [], label='p2d')[0]]
            lines += [trigger_ax.plot([], [], 'r--', label='trigger')[0]]
            #top_left_ax.legend()

            # Plot p2 (6,7,8) and trigger in top-right
            lines += [top_right_ax.plot([], [], label='p3m_raw')[0],
                      top_right_ax.plot([], [], label='p3m')[0],
                      top_right_ax.plot([], [], label='p3d')[0],
                      top_right_ax.plot([], [], label='p4m_raw')[0],
                      top_right_ax.plot([], [], label='p4m')[0],
                      top_right_ax.plot([], [], label='p4d')[0]]
            #top_right_ax.legend()

            # u1 (4) in bottom-left
            lines += [bottom_left_ax.plot([], [], label='u1')[0],
                      bottom_left_ax.plot([], [], label='u2')[0]]
           # bottom_left_ax.legend()

            # u2 (9) in bottom-right
            lines += [bottom_right_ax.plot([], [], label='u3')[0],
                      bottom_right_ax.plot([], [], label='u4')[0]]
           # bottom_right_ax.legend()

            axes_initialized = True

        x = np.append(x, time)[-300:]
        for idx in range(len(values)):
            y_data[idx] = np.append(y_data[idx], values[idx])[-300:]


        if axes_initialized and len(x) > 1:
            for ax in [top_left_ax, top_right_ax, bottom_left_ax, bottom_right_ax]:
                ax.set_xlim(x[0], x[-1])

            # ---- correct mapping ----
            lines[0].set_data(x, y_data[1])   # p1m_raw
            lines[1].set_data(x, y_data[2])   # p1m
            lines[2].set_data(x, y_data[3])   # p1d
            lines[3].set_data(x, y_data[5])   # p2m_raw
            lines[4].set_data(x, y_data[6])   # p2m
            lines[5].set_data(x, y_data[7])   # p2d
            lines[6].set_data(x, y_data[17])  # trigger (twin y-axis)

            lines[7].set_data(x, y_data[9])   # p3m_raw
            lines[8].set_data(x, y_data[10])  # p3m
            lines[9].set_data(x, y_data[11])  # p3d
            lines[10].set_data(x, y_data[13]) # p4m_raw  (FIX: was 11)
            lines[11].set_data(x, y_data[14]) # p4m
            lines[12].set_data(x, y_data[15]) # p4d

            lines[13].set_data(x, y_data[4])  # u1
            lines[14].set_data(x, y_data[8])  # u2
            lines[15].set_data(x, y_data[12]) # u3
            lines[16].set_data(x, y_data[16]) # u4
            # ---- end mapping ----

            # Recompute data limits and autoscale y each frame
            for ax in (top_left_ax, top_right_ax, bottom_left_ax, bottom_right_ax):
                ax.relim()
                ax.autoscale_view(scalex=False, scaley=True)
                ax.margins(y=0.05)  # small padding

            # twin y-axis for trigger needs its own relim/autoscale
            trigger_ax.relim()
            trigger_ax.autoscale_view(scalex=False, scaley=True)

    except Exception as e:
        print(f"Error processing packet: {e}")
        return lines

    return lines

anim = animation.FuncAnimation(fig, animate, init_func=init, interval=33, blit=False)
plt.tight_layout()
plt.show()
