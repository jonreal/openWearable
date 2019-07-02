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


x = np.linspace(-3.0, 0, 100)
y1 = np.multiply(x, 0.0)
y2 = np.multiply(x, 0.0)
y3 = np.multiply(x, 0.0)
y4 = np.multiply(x, 0.0)
y5 = np.multiply(x, 0.0)
y6 = np.multiply(x, 0.0)
y7 = np.multiply(x, 0.0)
y8 = np.multiply(x, 0.0)
y9 = np.multiply(x, 0.0)
y10 = np.multiply(x, 0.0)

fig, (ax1, ax2, ax3) = plt.subplots(3,1)
ax1.set_xlim([-3, 0])
ax1.set_ylim([0, 5])
ax2.set_xlim([-3, 0])
ax2.set_ylim([0, 5])
ax3.set_xlim([-3, 0])
ax3.set_ylim([0, 5])
ln1, = ax1.plot([], [], lw=2)
ln2, = ax1.plot([], [], lw=2)
ln3, = ax1.plot([], [], lw=2)
ln4, = ax1.plot([], [], lw=2)
ln5, = ax2.plot([], [], lw=2)
ln6, = ax2.plot([], [], lw=2)
ln7, = ax3.plot([], [], lw=2)
ln8, = ax3.plot([], [], lw=2)
ln9, = ax1.plot([], [], lw=2)
ln10, = ax1.plot([], [], lw=2)



ymin = 0
ymax = 60

def init():
    ln1.set_data([], [])
    ln2.set_data([], [])
    ln3.set_data([], [])
    ln4.set_data([], [])
    ln5.set_data([], [])
    ln6.set_data([], [])
    ln7.set_data([], [])
    ln8.set_data([], [])
    ln9.set_data([], [])
    ln10.set_data([], [])
    return ln1, ln2, ln3, ln4, ln5, ln6, ln7, ln8, ln9, ln10

def animate(i):
    global y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, x, ax1, ax2

    while (1) :
        try :
            n, add  = serverSock.recvfrom(128)
            val = n.decode("utf-8").split('\t')

            # Format mesg
            time = float(val[0])
            p_res = float(val[1])
            p_d1 = float(val[2])
            p_m1 = float(val[3])
            p_d2 = float(val[4])
            p_m2 = float(val[5])
            dp_m1 = float(val[6])
            dp_m2 = float(val[7])
            u1 = float(val[8])
            u2 = float(val[9])
            p1_max = float(val[10])
            p2_max = float(val[11])

            #print(data)
            print(time,'\t',
                  p_res,'\t',
                  p_d1,'\t',
                  p_m1,'\t',
                  p_d2,'\t',
                  p_m2,'\t',
                  dp_m1,'\t',
                  dp_m2,'\t',
                  u1,'\t',
                  u2,'\t',
                  p1_max,'\t',
                  p2_max,'\t',
                  );

            # update buffers
            y1 = np.concatenate([y1[1:],np.array([p_d1])])
            y2 = np.concatenate([y2[1:],np.array([p_m1])])
            y3 = np.concatenate([y3[1:],np.array([p_d2])])
            y4 = np.concatenate([y4[1:],np.array([p_m2])])

            y5 = np.concatenate([y5[1:],np.array([dp_m1])])
            y6 = np.concatenate([y6[1:],np.array([dp_m2])])

            y7 = np.concatenate([y7[1:],np.array([u1])])
            y8 = np.concatenate([y8[1:],np.array([u2])])

            y9 = np.concatenate([y9[1:],np.array([p1_max])])
            y10 = np.concatenate([y10[1:],np.array([p2_max])])


            ymin = np.amin(np.concatenate([y1,y2,y3,y4,y9,y10]))
            ymax = np.amax(np.concatenate([y1,y2,y3,y4,y9,y10]))
            ax1.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

            ymin = np.amin(np.concatenate([y5,y6]))
            ymax = np.amax(np.concatenate([y5,y6]))
            ax2.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

            ymin = np.amin(np.concatenate([y7,y8]))
            ymax = np.amax(np.concatenate([y7,y8]))
            ax3.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

        except os.error as e:
            if e.errno == errno.EAGAIN :
                break;
            else :
                raise e

    ln1.set_data(x,y1)
    ln2.set_data(x,y2)
    ln3.set_data(x,y3)
    ln4.set_data(x,y4)
    ln5.set_data(x,y5)
    ln6.set_data(x,y6)
    ln7.set_data(x,y7)
    ln8.set_data(x,y8)
    ln9.set_data(x,y9)
    ln10.set_data(x,y10)
    return ln1, ln2, ln3, ln4, ln5, ln6, ln7, ln8, ln9, ln10, ax1, ax2 

# ---



anim = animation.FuncAnimation(fig, animate, init_func=init,
                               interval=33, blit=False)
plt.show()





