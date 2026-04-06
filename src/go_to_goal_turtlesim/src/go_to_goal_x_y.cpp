#include <cmath>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <turtlesim/Pose.h>

using namespace std;

// Key variable declarations
geometry_msgs::Twist velocity_command;
geometry_msgs::Pose2D current;
geometry_msgs::Pose2D waypoint_goal;
ros::Publisher velocity_publisher;

// The gain K, which is used to calculate the linear velocity.
const double K_l = 0.5;

// The gain K, which is used to calculate the angular velocity.
const double K_a = 0.5;

// The distance threshold in meters that will determine when
// the turtlesim robot successfully reaches the goal.
const double distance_tolerance = 0.001;

// The angle threshold in radians that will determine when
// the turtlesim robot successfully reaches the goal.
const double angle_tolerance = 0.001;

// This flag determines when the robot needs to either
// move towards a waypoint or stop.
bool go_to_waypoint = false;

// Initialize variables and take care of other setup tasks.
void setup() {
    
    // We initialize with the default starting coordinate
    // for the waypoint simulator.
    waypoint_goal.x = 5.544445;
    waypoint_goal.y = 5.544445;

    // Initialize the Twist message.
    velocity_command.linear.x = 0.0;
    velocity_command.linear.y = 0.0;
    velocity_command.linear.z = 0.0;
    velocity_command.angular.x = 0.0;
    velocity_command.angular.y = 0.0;
    velocity_command.angular.z = 0.0;
}

double getDistanceToWaypoint() {
    return sqrt(pow(waypoint_goal.x - current.x, 2) + pow(waypoint_goal.y - current.y, 2));
}

// Get the heading error,
// i.e. how many radians does the robot need
// to turn the head towards the waypoint.
double getHeadingError() {
    double delta_x = waypoint_goal.x - current.x;
    double delta_y = waypoint_goal.y - current.y;
    double waypoint_heading = atan2(delta_y, delta_x);
    double heading_error = waypoint_heading - current.theta;

    if (heading_error > M_PI) {
        heading_error = heading_error - (2 * M_PI);
    } else if (heading_error < -M_PI) {
        heading_error = heading_error + (2 * M_PI);
    }

    return heading_error;
}

// If we haven't yet reached the goal, set the velocity value.
// Otherwise, stop the robot.
void setVelocity() {
    double distance_to_waypoint = getDistanceToWaypoint();
    double heading_error = getHeadingError();

    // If we are not yet at the waypoint.
    if (go_to_waypoint && (abs(distance_to_waypoint) > distance_tolerance)) {
        // If the robot's heading is off, fix it.
        if (abs(heading_error) > angle_tolerance) {
            velocity_command.linear.x = 0.0;
            velocity_command.angular.z = K_a * heading_error;
        } else {
            // Just fix the distance gap between current pose and waypoint.
            // The magnitude of the robot's velocity is directly
            // proportional to the distance the robot is from the goal.

            velocity_command.linear.x = K_l * distance_to_waypoint;
            velocity_command.angular.z = 0.0;
        }
    } else {
        ROS_INFO("Goal has been reached!");
        velocity_command.linear.x = 0.0;
        velocity_command.angular.z = 0.0;
        go_to_waypoint = false;
    }
}

// This callback function updates the current position and
// orientation of the robot.
void updatePose(const turtlesim::PoseConstPtr &current_pose) {
    current.x = current_pose->x;
    current.y = current_pose->y;
    current.theta = current_pose->theta;
}

// This callback function updates the desired waypoint
// when a waypoint message is published to the /waypoint topic.
void updateWaypoint(const geometry_msgs::Pose2D &waypoint_pose) {
    waypoint_goal.x = waypoint_pose.x;
    waypoint_goal.y = waypoint_pose.y;
    go_to_waypoint = true;
}

int main(int argc, char **argv) {
    setup();

    // Initiate ROS
    ros::init(argc, argv, "go_to_goal_x_y");

    auto node_handle = ros::NodeHandle();

    // Subscribe to the robot's pose
    // Hold no messages in the queue. Automatically throw away 
    // any messages that are received that are not able to be
    // processed quickly enough.
    // Every time a new pose is received, update the robot's pose.
    auto current_pose_subscriber = node_handle.subscribe("turtle1/pose", 0, updatePose);

    // Subscribe to the robot's pose
    // Hold no messages in the queue. Automatically throw away 
    // any messages that are received that are not able to be
    // processed quickly enough.
    // Every time a new pose is received, update the robot's pose.
    // The tcpNoDelay is to reduce latency between nodes and to make sure we are
    // not missing any critical waypoint messages.
    auto waypoint_pose_subscriber = node_handle.subscribe("waypoint", 0, updateWaypoint, ros::TransportHints().tcpNoDelay());

    velocity_publisher = node_handle.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 0);

    auto rate = ros::Rate(10.0);
    while (node_handle.ok()) {
        
        // Here is where we call the callbacks that need to be called.
        ros::spinOnce();

        // After we call the callback function to update the robot's pose,
        // we set the velocity values for the robot.
        setVelocity();

        // Publish the velocity command to the ROS topic.
        velocity_publisher.publish(velocity_command);

        ROS_INFO("Current x = %lf\nDesired x = %lf\nDistance to goal = %lf m\nLinear velocity (x) = %lf m/s",
            current.x,
            waypoint_goal.x,
            getDistanceToWaypoint(),
            velocity_command.linear.x
        );

        rate.sleep();
    }
}
