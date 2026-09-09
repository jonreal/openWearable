"""Host -> board command sender (the return channel).

Browsers can't speak UDP, so the hub is the translator: it takes control messages
off the WebSocket and sends them to the board's command channel as ASCII lines
"<name> <value>\\n" on udp:1501 (see library/a8/src/cmd.c). The board enforces the
single arm gate; this class just forwards. Idempotent level-sets -- the board
echoes state back in telemetry, which the UI uses to confirm.
"""

import socket

CMD_PORT = 1501


class Commander:
    def __init__(self, host, port=CMD_PORT):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send(self, name, value):
        """Send one command line. Returns False (never raises) if there's no board
        host yet or the host doesn't resolve, so a bad target can't break control."""
        if not self.host:
            return False
        try:
            self.sock.sendto(f"{name} {float(value)}\n".encode("ascii"), (self.host, self.port))
            return True
        except OSError:                      # incl. gaierror (unresolvable host)
            return False

    def arm(self, on):
        return self.send("arm", 1 if on else 0)
