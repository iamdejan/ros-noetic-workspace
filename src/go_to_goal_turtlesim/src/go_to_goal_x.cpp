#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <turtlesim/Pose.h>

using namespace std;

// Key variable declarations
geometry_msgs::Twist velocity_command;
geometry_msgs::Pose2D current;
geometry_msgs::Pose2D desired;

// Goal x-value, which can be any number from 0-11 (inclusive).
const double GOAL = 1.3;

// The gain K, which is used to calculate the linear velocity.
const double K_l = 1.0;

// The distance threshold in meters that will determine when
// the turtlesim robot successfully reaches the goal.
const double distance_tolerance = 0.001;

// Initialize variables and take care of other setup tasks.
void setup() {

    // Desired x goal coordinate
    desired.x = GOAL;

    // Initialize the twist message.
    // Initial linear and angular velocities are 0 m/s and rad/s, respectively.
    velocity_command.linear.x = 0.0;
    velocity_command.linear.y = 0.0;
    velocity_command.linear.z = 0.0;
    velocity_command.angular.x = 0.0;
    velocity_command.angular.y = 0.0;
    velocity_command.angular.z = 0.0;
}

// Get the distance between the current x coordinate and the desired x coordinate.
double getDistanceToGoal() {
    return desired.x - current.x;
}

// If we haven't yet reached the goal, set the velocity value.
// Otherwise, stop the robot.
void setVelocity() {
    if (abs(getDistanceToGoal()) > distance_tolerance) {

        // The magnitude of the robot's velocity is
        // directly proportional to the distance the robot is from the goal.
        velocity_command.linear.x = K_l * getDistanceToGoal();
    } else {
        ROS_INFO("Goal has been reached!");
        velocity_command.linear.x = 0.0;
    }
}

// This callback function updates the current position and orientation of the robot.
void updatePose(const turtlesim::PoseConstPtr &current_pose) {
    current.x = current_pose->x;
    current.y = current_pose->y;
    current.theta = current_pose->theta;
}

int main(int argc, char **argv) {
    setup();

    // Initiate ROS
    ros::init(argc, argv, "go_to_goal_x");

    // Create the main access point to communicate with ROS.
    auto node_handle = ros::NodeHandle();

    // Subscribe to the robot's pose.
    // Hold no messages in the queue. Automatically throw away
    // any message(s) that are received that are not able to be processed
    // quick enough.
    // Every time a new pose is received, update the robot's pose.
    auto current_pose_sub = node_handle.subscribe("turtle1/pose", 0, updatePose);

    // Publish velocity commands to a topic.
    // Hold no messages in the queue. Automatically throw away
    // any message(s) that are received that are not able to be processed
    // quick enough.
    auto velocity_publisher = node_handle.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 0);

    // Specify a frequency for the loop.
    // In this case, we want to loop 10 cycles per second.
    auto rate = ros::Rate(10.0);

    while (node_handle.ok()) {
        
        // Here is where we call the callbacks that need to be called.
        ros::spinOnce();

        // After we call the callback function to update the robot's pose,
        // we set the velocity values for the robot.
        setVelocity();

        // Publish the velocity command to the ROS topic.
        velocity_publisher.publish(velocity_command);

        ROS_INFO("Current x = %lf\nDesired x = %lf\nDistance to goal = %lf m\nLinear velocity (x) = %lf m/s", current.x, desired.x, getDistanceToGoal(), velocity_command.linear.x);

        rate.sleep();
    }
}
