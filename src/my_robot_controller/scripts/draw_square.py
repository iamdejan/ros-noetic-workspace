#!/usr/bin/env python3

from enum import Enum
import math

import rospy
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose


DELTA = 1e-2
SERVICE_NAME = "/turtle1/set_pen"
THETA_TARGETS = [0.0, math.radians(90), math.radians(180-360), math.radians(270-360)]
FORWARD_SPEED = 4.0
TURN_SPEED = 0.1


class State(Enum):
    STILL = 0
    FORWARD = 1
    TURN_LEFT = 2


current_state = State.STILL
current_target_index = 0


def need_to_turn_left(pose: Pose) -> bool:
    if abs(pose.theta - THETA_TARGETS[0]) <= DELTA or abs(pose.theta - THETA_TARGETS[2]) <= DELTA:
        return pose.x > 9.0 or pose.x < 2.0
    if abs(pose.theta - THETA_TARGETS[1]) <= DELTA or abs(pose.theta - THETA_TARGETS[3]) <= DELTA:
        return pose.y > 9.0 or pose.y < 2.0
    return False


def pose_callback(pose: Pose, publisher: rospy.Publisher):
    command = Twist()

    global current_state
    global current_target_index
    theta_target = THETA_TARGETS[current_target_index]
    rospy.loginfo(f"state = {current_state}, pose = ({pose.x:.4f}, {pose.y:.4f}), pose.theta = {pose.theta:.4f}, theta_target = {theta_target:.4f}")
    if current_state == State.STILL and abs(pose.theta - theta_target) <= DELTA:
        command.linear.x = FORWARD_SPEED
        command.linear.z = 0.0
        current_state = State.FORWARD
    elif current_state == State.FORWARD and need_to_turn_left(pose):
        command.linear.x = 0.0
        current_state = State.STILL
        current_target_index = (current_target_index + 1) % len(THETA_TARGETS)
    elif current_state == State.STILL and abs(pose.theta - theta_target) > DELTA:
        command.angular.z = TURN_SPEED
        current_state = State.TURN_LEFT
    elif current_state == State.TURN_LEFT and abs(pose.theta - theta_target) <= DELTA:
        command.linear.z = 0.0
        current_state = State.STILL
    elif current_state == State.FORWARD:
        command.linear.x = FORWARD_SPEED
    elif current_state == State.TURN_LEFT:
        command.angular.z = TURN_SPEED

    publisher.publish(command)


def main():
    rospy.init_node("draw_square")
    rospy.wait_for_service(SERVICE_NAME)

    publisher = rospy.Publisher("/turtle1/cmd_vel", Twist, queue_size=10)
    rospy.Subscriber("/turtle1/pose", Pose, callback=pose_callback, callback_args=publisher)

    rospy.spin()


if __name__ == "__main__":
    main()
