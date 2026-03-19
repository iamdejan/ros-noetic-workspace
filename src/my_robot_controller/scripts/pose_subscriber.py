#!/usr/bin/env python3

import rospy
from turtlesim.msg import Pose


def pose_callback(msg: Pose):
    rospy.loginfo(f"({msg.x:.3f}, {msg.y:.3f})")


if __name__ == "__main__":
    rospy.init_node("pose_subscriber")
    sub = rospy.Subscriber("/turtle1/pose", Pose, callback=pose_callback)
    
    rospy.loginfo("Node has been started")

    rospy.spin()
