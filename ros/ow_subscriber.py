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
    buttons = int(val[2])
    print(frame, state, buttons)

def listener():
    rospy.init_node('listener', anonymous=True)
    rospy.Subscriber('openwearable', String, callback)
    rospy.spin()

if __name__ == '__main__':
    listener()
