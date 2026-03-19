#include <ros/ros.h>
#include <turtlesim/Pose.h>
#include <string>

class Node {
public:
    void callback(turtlesim::Pose msg) {
        std::string msg_string = "(" + std::to_string(msg.x) + ", " + std::to_string(msg.y) + ")";
        ROS_INFO(msg_string.c_str());
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_pose_subscriber");
    auto nh = ros::NodeHandle(); // Create the NodeHandle (calls ros::start())

    auto node = Node();
    auto sub = nh.subscribe<turtlesim::Pose>("/turtle1/pose", 10, &Node::callback, &node);

    ROS_INFO("Node has been started");

    ros::spin();
}
