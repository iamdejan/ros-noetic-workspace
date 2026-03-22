#!/usr/bin/env python3

from enum import Enum
import math

import rospy
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose


DELTA = 1e-2
"""
Tolerance rate for float numbers' comparison.
"""

THETA_TARGETS = (0.0, math.radians(90), math.radians(180-360), math.radians(270-360))
"""
A tuple that represent different theta values which denote the robot's direction.
If the robot has reached a theta target, stop rotation.

Values
------
    * 0.00: The robot is facing east.
    * 1.57: The robot is facing north.
    * -3.14: The robot is facing west.
    * -1.57: The robot is facing south.
"""

FORWARD_SPEED = 4.0
"""
Denotes how fast the robot moves forward.
"""

TURN_SPEED = 0.2
"""
Denotes how fast the robot turns.
"""

UPDATE_BY_DELTA_MULTIPLIER = 0.0
"""
x_limits and y_limits will be updated when the turtle turns left.
This variable controls how much the limits should be widened from pose.x or pose.y, relative to the delta.
This can serve as a tolerance, since we cannot turn the robot perfectly 90 degrees.
"""


class State(Enum):
    """
    State enum representing different states of the turtle.
    The state will be used to decide the next course of action.

    Attributes
    ----------
    STILL: int
        The turtle is not moving.
    FORWARD: int
        The turtle is moving forward.
    TURN_LEFT: int
        The turtle is moving left. Note that we do not need right turn to draw the square.
    """

    STILL = 0
    FORWARD = 1
    TURN_LEFT = 2


current_state: State = State.STILL
"""
Current state of the turtle.
"""

current_target_index = 0
"""
Index of the current theta target.
Different theta target will determine whether/not the turtle has finished rotating.
"""

x_limits = [2.0, 9.0]
y_limits = [2.0, 9.0]


def need_to_turn_left(pose: Pose) -> bool:
    global x_limits
    global y_limits

    if abs(pose.theta - THETA_TARGETS[0]) <= DELTA or abs(pose.theta - THETA_TARGETS[2]) <= DELTA:
        return pose.x < x_limits[0] or pose.x > x_limits[1]
    if abs(pose.theta - THETA_TARGETS[1]) <= DELTA or abs(pose.theta - THETA_TARGETS[3]) <= DELTA:
        return pose.y < y_limits[0] or pose.y > y_limits[1]
    return False


def update_limits(pose: Pose):
    """
    Due to imprecision, we have to update limits every time we turn.
    This function will update the limits according to the given pose.
    Basically, pose.x and pose.y will be the new limit for next checking.

    Params
    ------
    pose: Pose
        The current coordinate (x, y), direction (theta), and velocities of the turtle.
    """

    global x_limits
    global y_limits

    x_limits[0] = min(x_limits[0], pose.x-UPDATE_BY_DELTA_MULTIPLIER*DELTA)
    x_limits[1] = max(x_limits[1], pose.x+UPDATE_BY_DELTA_MULTIPLIER*DELTA)

    y_limits[0] = min(y_limits[0], pose.y-UPDATE_BY_DELTA_MULTIPLIER*DELTA)
    y_limits[1] = max(y_limits[1], pose.y+UPDATE_BY_DELTA_MULTIPLIER*DELTA)
    pass


def get_formatted_limits() -> str:
    """
    Format the limits, then return as string.
    """

    global x_limits
    global y_limits

    x_limits_str: str = ", ".join([f"{item:.4f}" for item in x_limits])
    y_limits_str: str = ", ".join([f"{item:.4f}" for item in y_limits])
    return f"x_limits = [{x_limits_str}], y_limits = [{y_limits_str}]"


def pose_callback(pose: Pose, publisher: rospy.Publisher):
    """
    Callback function that receives the pose of the turtle, then act on it.
    This function serves as a close-loop feedback, where the pose received
    by the subscriber affects the command sent by publisher.

    Params
    ------
    pose: Pose
        The current coordinate (x, y), direction (theta), and velocities of the turtle.
    publisher: rospy.Publisher
        The publisher that allows us to send commands to the turtle.
    """

    global current_state
    global current_target_index

    command = Twist()
    theta_target = THETA_TARGETS[current_target_index]
    rospy.loginfo(f"state = {current_state}, pose = ({pose.x:.4f}, {pose.y:.4f}), pose.theta = {pose.theta:.4f}, theta_target = {theta_target:.4f}, {get_formatted_limits()}")

    if current_state == State.STILL and abs(pose.theta - theta_target) <= DELTA:
        command.linear.x = FORWARD_SPEED
        command.angular.z = 0.0
        current_state = State.FORWARD
    elif current_state == State.FORWARD and need_to_turn_left(pose):
        command.linear.x = 0.0
        current_state = State.STILL
        current_target_index = (current_target_index + 1) % len(THETA_TARGETS)
    elif current_state == State.STILL and abs(pose.theta - theta_target) > DELTA:
        update_limits(pose)

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

    publisher = rospy.Publisher("/turtle1/cmd_vel", Twist, queue_size=10)
    rospy.Subscriber("/turtle1/pose", Pose, callback=pose_callback, callback_args=publisher)

    rospy.spin()


if __name__ == "__main__":
    main()
