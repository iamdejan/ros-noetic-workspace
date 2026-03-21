#!/usr/bin/env python3

import rospy
from std_msgs.msg import String


def main():
    rospy.init_node("radio_transmitter", anonymous=True)
    publisher = rospy.Publisher("/robot_news_radio", String, queue_size=10)
    rate = rospy.Rate(10)

    rospy.loginfo("Node has been started")

    while not rospy.is_shutdown():
        msg = String()
        msg.data = "Hi, this is radio_transmitter node speaking!"
        publisher.publish(msg)
        rate.sleep()


if __name__ == "__main__":
    main()
