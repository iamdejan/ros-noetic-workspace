#include <ros/ros.h>
#include <std_msgs/String.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "radio_transmitter", ros::InitOption::AnonymousName);
    auto nh = ros::NodeHandle();
    auto pub = nh.advertise<std_msgs::String>("/robot_news_radio", 10);
    auto rate = ros::Rate(10);

    ROS_INFO("Node has been started");

    while (ros::ok()) {
        auto msg = std_msgs::String();
        msg.data = "Hi, this is radio_transmitter node speaking!";
        pub.publish(msg);
        rate.sleep();
    }
}
