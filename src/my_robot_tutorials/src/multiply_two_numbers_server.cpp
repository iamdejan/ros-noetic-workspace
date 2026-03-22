#include <string>

#include <ros/ros.h>
#include "my_robot_tutorials/MultiplyTwoNumbers.h"

const std::string SERVICE_NAME = "multiply_two_numbers";

bool multiply(my_robot_tutorials::MultiplyTwoNumbers::Request &request,
              my_robot_tutorials::MultiplyTwoNumbers::Response &response) {
    response.result = request.a * request.b;
    return true;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, (SERVICE_NAME + "_server").c_str());
    auto nh = ros::NodeHandle();

    // Save to a variable, even though we don't use it.
    // This is due to C++ RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with Rust.
    auto sub = nh.advertiseService(SERVICE_NAME.c_str(), multiply);

    ROS_INFO("Server has been started.");
    ROS_INFO("Ready to multiply two numbers.");

    ros::spin();
}
