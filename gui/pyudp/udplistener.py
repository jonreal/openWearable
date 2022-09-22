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
while (1) :
  while (1) :
    try :
        n, add  = serverSock.recvfrom(128)
        val = n.decode("utf-8").split('\t')

        # Format mesg
        time = float(val[0])
        v1 = float(val[1])
        v2 = float(val[2])
        v3 = float(val[3])

        print(time,'\t',
              v1,'\t',
              v2,'\t',
              v3,'\t')

    except os.error as e:
        if e.errno == errno.EAGAIN :
            break;
        else :
            raise e
