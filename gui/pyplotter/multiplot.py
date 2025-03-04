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

fig, ax = plt.subplots()

# Default axis limits
ax.set_xlim(-10, 0)
ax.set_ylim(0, 60)

lines = []  # Will hold the line objects

def init():
    """Initialize the plot lines."""
    global lines
    for line in lines:
        line.set_data([], [])
    return lines

def receive_latest_packet():
    """Fetch the latest available UDP packet, discarding older ones."""
    global latest_packet
    while True:
        try:
            latest_packet, _ = serverSock.recvfrom(128)
        except os.error as e:
            if e.errno == errno.EAGAIN:
                break
            else:
                raise e

def animate(i):
    global x, y_data, ax, lines, latest_packet

    # Fetch the latest available packet before processing
    receive_latest_packet()
    
    if latest_packet is None:
        return lines  # No new data, keep previous frame

    try:
        val = latest_packet.decode("utf-8").replace("\x00", "").strip().split('\t')

        # Ensure at least one element for time
        time = float(val[0]) / 1000.0

        # Convert the rest of the values dynamically
        values = [float(v) for v in val[1:] if v.strip()]
        num_signals = len(values)

        # Print received values
        print(f"Time: {time:.3f}, Values: {values}")

        # If needed, initialize y_data and lines dynamically
        if len(y_data) != num_signals:
            y_data = [np.array([]) for _ in range(num_signals)]
            ax.clear()
            lines.clear()
            for _ in range(num_signals):
                line, = ax.plot([], [], lw=2)
                lines.append(line)

        # Update time buffer
        x = np.append(x[-299:], time)

        # Update y buffers dynamically
        for idx in range(num_signals):
            y_data[idx] = np.append(y_data[idx][-299:], values[idx])

        # Update y-axis limits dynamically
        ymin = min(min(y) for y in y_data) if y_data else 0
        ymax = max(max(y) for y in y_data) if y_data else 60
        ax.set_ylim(ymin - 0.1 * (ymax - ymin), ymax + 0.1 * (ymax - ymin))

        # Update x-axis limits dynamically
        xmin, xmax = np.amin(x), np.amax(x)
        ax.set_xlim(xmin, xmax)

    except Exception as e:
        print(f"Error processing packet: {e}")
        return lines

    # Update plot data
    for idx, line in enumerate(lines):
        line.set_data(x, y_data[idx])

    return lines

# Run animation
anim = animation.FuncAnimation(fig, animate, init_func=init, interval=33, blit=False)
plt.show()
