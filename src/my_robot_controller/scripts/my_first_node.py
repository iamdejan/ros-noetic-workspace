#!/usr/bin/env python3
import rospy


def main():
    rospy.init_node("test_node")
    rospy.loginfo("Test node has been started")

    rate = rospy.Rate(10)
    while not rospy.is_shutdown():
        rospy.loginfo("Hello")
        rate.sleep()


if __name__ == "__main__":
    main()
