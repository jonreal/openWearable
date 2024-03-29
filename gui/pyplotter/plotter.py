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


x = np.linspace(-1, 0, 300)
y1 = np.multiply(x, 0.0)
y2 = np.multiply(x, 0.0)
y3 = np.multiply(x, 0.0)

fig = plt.figure()
ax = plt.axes(xlim=(-10.,0), ylim=(0,60))
ln1, = ax.plot([], [], lw=2)
ln2, = ax.plot([], [], lw=2)
ln3, = ax.plot([], [], lw=2)

ymin = 0
ymax = 60

def init():
    ln1.set_data([], [])
    ln2.set_data([], [])
    ln3.set_data([], [])
    return ln1, ln2, ln3

def animate(i):
    global y1, y2, y3, x, ax

    while (1) :
        try :
            n, add  = serverSock.recvfrom(128)
            val = n.decode("utf-8").split('\t')

            # Format mesg
            time = float(val[0])/1000.0
            v1 = float(val[1])
            v2 = float(val[2])
            v3 = float(val[3])

            #print(data)
            print(time,'\t',
                  v1,'\t',
                  v1,'\t',
                  v3,'\t')

            # update buffers
            x = np.concatenate([x[1:],np.array([time])])
            y1 = np.concatenate([y1[1:],np.array([v1])])
            y2 = np.concatenate([y2[1:],np.array([v2])])
            y3 = np.concatenate([y3[1:],np.array([v3])])

            ymin = np.amin(np.concatenate([y1,y2,y3]))
            ymax = np.amax(np.concatenate([y1,y2,y3]))
            ax.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

            xmin = np.amin(x);
            xmax = np.amax(x);
            ax.set_xlim([xmin,xmax])

        except os.error as e:
            if e.errno == errno.EAGAIN :
                break;
            else :
                raise e

    ln1.set_data(x,y1)
    ln2.set_data(x,y2)
    ln3.set_data(x,y3)
    return ln1, ln2, ln3, ax 

# ---



anim = animation.FuncAnimation(fig, animate, init_func=init,
                               interval=33, blit=False)
plt.show()





