#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_draw_circle");
    auto nh = ros::NodeHandle(); // Create the NodeHandle (calls ros::start())

    ROS_INFO("Node has been started");

    auto publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    auto rate = ros::Rate(2.0);
    while (ros::ok()) {
        auto message = geometry_msgs::Twist();
        message.linear.x = 2.0;
        message.angular.z = 1.0;
        publisher.publish(message);

        rate.sleep();
    }
}
