#include <ros/ros.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_my_first_node");
    auto node_handle = ros::NodeHandle(); // Create the NodeHandle (calls ros::start())

    ROS_INFO("Node has been started");

    ros::Rate rate = ros::Rate(10.0);
    while (ros::ok()) {
        ROS_INFO("Hello");
        rate.sleep();
    }
}
