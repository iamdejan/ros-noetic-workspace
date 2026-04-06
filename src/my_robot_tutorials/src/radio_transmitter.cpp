#include <ros/ros.h>
#include <std_msgs/String.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "radio_transmitter", ros::InitOption::AnonymousName);
    auto node_handle = ros::NodeHandle();
    auto publisher = node_handle.advertise<std_msgs::String>("/robot_news_radio", 10);
    auto rate = ros::Rate(10);

    ROS_INFO("Node has been started");

    while (ros::ok()) {
        auto msg = std_msgs::String();
        msg.data = "[C++] Hi, this is radio_transmitter node speaking!";
        publisher.publish(msg);
        rate.sleep();
    }
}
