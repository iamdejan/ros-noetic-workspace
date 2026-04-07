#!/usr/bin/env python3

from math import pow, atan2, sqrt

import rospy
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose


DISTANCE_TOLERANCE = 0.001


class TurtleBot:
    def __init__(self):
        # Create a node with name "turtlebot_controller" and make sure
        # it is a unique node (using anonymous=True).
        rospy.init_node("turtlebot_controller", anonymous=True)

        # Publisher which will publish to the topic "/turtle1/cmd_vel".
        self.velocity_publisher = rospy.Publisher("/turtle1/cmd_vel", Twist, queue_size=10)

        # A subscriber to the topic "/turtle1/pose". self.update_pose is called
        # when a message to type Pose is received.
        self.pose_subscriber = rospy.Subscriber("/turtle1/pose", Pose, self.update_pose)

        self.pose = Pose()
        self.rate = rospy.Rate(10.0)


    def update_pose(self, pose: Pose) -> None:
        self.pose = pose
        self.pose.x = round(self.pose.x, 4)
        self.pose.y = round(self.pose.y, 4)


    def euclidean_distance(self, goal_pose: Pose) -> float:
        return sqrt(pow(self.pose.x - goal_pose.x, 2) + pow(self.pose.y - goal_pose.y, 2))


    def linear_velocity(self, goal_pose: Pose, constant=1.5) -> float:
        return constant * self.euclidean_distance(goal_pose)


    def steering_angle(self, goal_pose: Pose) -> float:
        return atan2(goal_pose.y - self.pose.y, goal_pose.x - self.pose.x)


    def angular_velocity(self, goal_pose: Pose, constant=6.0) -> float:
        return constant * (self.steering_angle(goal_pose) - self.pose.theta)


    def move_to_goal(self):
        goal_pose = Pose()

        # Get the input from the user.
        goal_pose.x = float(input("Set your x goal: "))
        goal_pose.y = float(input("Set your y goal: "))

        velocity_message = Twist()

        while self.euclidean_distance(goal_pose) > DISTANCE_TOLERANCE:
            rospy.loginfo(f"distance to goal = {self.euclidean_distance(goal_pose)}")

            # Proportional controller: https://en.wikipedia.org/wiki/Proportional_control

            # Linear velocity in the x-axis.
            velocity_message.linear.x = self.linear_velocity(goal_pose)
            velocity_message.linear.y = 0.0
            velocity_message.linear.z = 0.0

            # Angular velocity in the z-axis.
            velocity_message.angular.x = 0.0
            velocity_message.angular.y = 0.0
            velocity_message.angular.z = self.angular_velocity(goal_pose)

            self.velocity_publisher.publish(velocity_message)

            self.rate.sleep()

        # Stopping our robot after the movement is over (goal is reached).
        velocity_message.linear.x = 0.0
        velocity_message.linear.z = 0.0
        self.velocity_publisher.publish(velocity_message)

        rospy.spin()


def main():
    try:
        x = TurtleBot()
        x.move_to_goal()
    except rospy.ROSInterruptException as e:
        rospy.logwarn(f"Interrupted: {e}")
        pass


if __name__ == "__main__":
    main()
