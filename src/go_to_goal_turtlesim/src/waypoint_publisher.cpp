#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>

using namespace std;

// Ask user for the desired waypoint.
geometry_msgs::Pose2D inputWaypoint() {
    auto waypoint = geometry_msgs::Pose2D();

    cout << "Where do you want the robot to go?" << endl;
    cout << "Enter waypoint x: ";
    cin >> waypoint.x;
    cout << "Enter waypoint y: ";
    cin >> waypoint.y;
    cout << endl;

    return waypoint;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "waypoint_publisher");

    // Create the main access point to communicate with ROS.
    auto node_handle = ros::NodeHandle();

    // Publish waypoint to a topic.
    // Hold no messages in the queue. Automatically throw away
    // any message(s) that are received that are not able to be processed
    // quick enough.
    auto waypoint_publisher = node_handle.advertise<geometry_msgs::Pose2D>("waypoint", 0);

    // Keep running the while loop below.
    while (node_handle.ok()) {
        auto waypoint = inputWaypoint();

        waypoint_publisher.publish(waypoint);
    }
}
