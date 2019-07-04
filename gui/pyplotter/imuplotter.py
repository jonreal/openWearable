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

fig, (ax1, ax2, ax3) = plt.subplots(3,1)
ax1.set_xlim([-3, 0])
ax1.set_ylim([0, 5])
ax2.set_xlim([-3, 0])
ax2.set_ylim([0, 5])
ax3.set_xlim([-3, 0])
ax3.set_ylim([0, 5])
ln1, = ax1.plot([], [], lw=2)
ln2, = ax1.plot([], [], lw=2)
ln3, = ax2.plot([], [], lw=2)
ln4, = ax2.plot([], [], lw=2)
ln5, = ax3.plot([], [], lw=2)
ln6, = ax3.plot([], [], lw=2)

ymin = 0
ymax = 60

def init():
    ln1.set_data([], [])
    ln2.set_data([], [])
    ln3.set_data([], [])
    ln4.set_data([], [])
    ln5.set_data([], [])
    ln6.set_data([], [])
    return ln1, ln2, ln3, ln4, ln5, ln6


def animate(i):
    global y1, y2, y3, y4, y5, y6, x, ax1, ax2, ax3

    while (1) :
        try :
            n, add  = serverSock.recvfrom(128)
            val = n.decode("utf-8").split('\t')

            # Format mesg
            time = float(val[0])
            imu1_roll = float(val[1])
            imu1_pitch = float(val[2])
            imu1_yaw = float(val[3])
            imu2_roll = float(val[4])
            imu2_pitch = float(val[5])
            imu2_yaw = float(val[6])


            #print(data)
            print(time,'\t',
                  imu1_roll,'\t',
                  imu1_pitch,'\t',
                  imu1_yaw,'\t',
                  imu2_roll,'\t',
                  imu2_pitch,'\t',
                  imu2_yaw,'\t',
                  );

            # update buffers
            y1 = np.concatenate([y1[1:],np.array([imu1_roll])])
            y2 = np.concatenate([y2[1:],np.array([imu1_pitch])])
            y3 = np.concatenate([y3[1:],np.array([imu1_yaw])])

            y4 = np.concatenate([y4[1:],np.array([imu2_roll])])
            y5 = np.concatenate([y5[1:],np.array([imu2_pitch])])
            y6 = np.concatenate([y6[1:],np.array([imu2_yaw])])

            ymin = np.amin(np.concatenate([y1,y4]))
            ymax = np.amax(np.concatenate([y1,y4]))
            ax1.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

            ymin = np.amin(np.concatenate([y2,y5]))
            ymax = np.amax(np.concatenate([y2,y5]))
            ax2.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

            ymin = np.amin(np.concatenate([y3,y6]))
            ymax = np.amax(np.concatenate([y3,y6]))
            ax3.set_ylim([(ymin - 0.1*(ymax-ymin)),(ymax + 0.1*(ymax-ymin))])

        except os.error as e:
            if e.errno == errno.EAGAIN :
                break;
            else :
                raise e

    ln1.set_data(x,y1)
    ln2.set_data(x,y4)
    ln3.set_data(x,y2)
    ln4.set_data(x,y5)
    ln5.set_data(x,y3)
    ln6.set_data(x,y6)
    return ln1, ln2, ln3, ln4, ln5, ln6, ax1, ax2, ax3

# ---

anim = animation.FuncAnimation(fig, animate, init_func=init,
                               interval=33, blit=False)
plt.show()



