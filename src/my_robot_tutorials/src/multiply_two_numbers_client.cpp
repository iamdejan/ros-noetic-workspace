#include <string>

#include <ros/ros.h>
#include "my_robot_tutorials/MultiplyTwoNumbers.h"

const std::string SERVICE_NAME = "/multiply_two_numbers";

std::string usage(char **argv) {
    return std::string(argv[0]) + " a b";
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "multiply_two_numbers_client");
    auto node_handle = ros::NodeHandle();

    if (argc != 3) {
        ROS_ERROR("%s", usage(argv).c_str());
        std::exit(1);
    }

    ros::service::waitForService(SERVICE_NAME);

    auto client = node_handle.serviceClient<my_robot_tutorials::MultiplyTwoNumbers>(SERVICE_NAME);

    auto srv = my_robot_tutorials::MultiplyTwoNumbers();
    srv.request.a = atof(argv[1]);
    srv.request.b = atof(argv[2]);
    ROS_INFO("Requesting %.3lf * %.3lf...", srv.request.a, srv.request.b);

    if (client.call(srv)) {
        ROS_INFO("%.3lf * %.3lf = %.3lf", srv.request.a, srv.request.b, srv.response.result);
    } else {
        ROS_ERROR("Service call failed");
    }
}
