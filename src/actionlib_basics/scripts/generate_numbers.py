#!/usr/bin/env python3

import random

import rospy
from std_msgs.msg import Float64


MEAN = 5.0
STANDARD_DEVIATION = 1.0
GOAL = 100


def main():
    rospy.init_node("generate_numbers")
    rospy.loginfo("Node has been started")

    publisher = rospy.Publisher("/random_number", Float64, queue_size=10)

    rate = rospy.Rate(5)

    sample_count = 1
    while not rospy.is_shutdown():
        number = random.normalvariate(MEAN, STANDARD_DEVIATION)
        publisher.publish(Float64(number))
        rospy.loginfo(f"[sample {sample_count}] Generate number {number}")

        if sample_count >= GOAL:
            break

        sample_count += 1
        rate.sleep()

    rospy.loginfo("Finished generating numbers")


if __name__ == "__main__":
    main()
