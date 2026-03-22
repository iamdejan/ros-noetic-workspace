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

    // Save to a variable, even though we don't use it.
    // This is due to C++ RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with Rust.
    auto sub = nh.subscribe<turtlesim::Pose>("/turtle1/pose", 10, &Node::callback, &node);

    ROS_INFO("Node has been started");

    ros::spin();
}
