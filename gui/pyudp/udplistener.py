import socket
import sys
import numpy as np
import fcntl, os
import errno

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 1500
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.bind(('', UDP_PORT_NO))
fcntl.fcntl(serverSock, fcntl.F_SETFL, os.O_NONBLOCK)

print("\n\nListening for UDP...\n")
while True:
    while True:
        try:
            n, add = serverSock.recvfrom(128)

            # Remove null bytes and strip whitespace
            cleaned_data = n.decode("utf-8").replace("\x00", "").strip()

            # Split into values
            val = cleaned_data.split('\t')

            # Ensure at least one element for time
            time = float(val[0]) if val else 0.0

            # Convert remaining elements to floats, ignoring empty values
            values = [float(v) for v in val[1:] if v.strip()]

            # Print formatted output
            print(time, '\t', '\t'.join(map(str, values)))

        except ValueError as e:
            print("ValueError:", e, "Received:", repr(n))  # Debugging output

        except os.error as e:
            if e.errno == errno.EAGAIN:
                break
            else:
                raise e
