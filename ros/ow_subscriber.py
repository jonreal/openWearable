#!/usr/bin/env python
import rospy
from std_msgs.msg import String

def callback(data):
    strdata = str(data)

    # hacky split
    val = strdata.split(':')
    val = val[1].split('\\t');
    temp = val[0].split('"');

    frame = int(temp[1])
    state = int(val[1])
    pm_pro = float(val[2])
    pd_pro = float(val[3])
    pm_sup = float(val[4])
    pd_sup = float(val[5])
    print(frame, state, pm_pro, pd_pro, pm_sup, pd_sup)

def listener():
    rospy.init_node('listener', anonymous=True)
    rospy.Subscriber('openwearable', String, callback)
    rospy.spin()

if __name__ == '__main__':
    listener()
