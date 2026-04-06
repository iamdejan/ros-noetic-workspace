#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <turtlesim/SetPen.h>
#include <string>

const auto SERVICE_NAME = "/turtle1/set_pen";

void call_set_pen_service(ros::NodeHandle nh, uint8_t r, uint8_t g, uint8_t b, uint8_t width, uint8_t off) {
    auto client = nh.serviceClient<turtlesim::SetPen>(SERVICE_NAME);
    auto request = turtlesim::SetPen::Request();
    request.r = r;
    request.g = g;
    request.b = b;
    request.width = width;
    request.off = off;
    auto srv = turtlesim::SetPen();
    srv.request = request;
    bool call_success = client.call(srv);
    if (!call_success) {
        auto log = std::string("Call failed for ") + std::string(SERVICE_NAME);
        ROS_WARN(log.c_str());
    }
}

class Node {
private:
    float previous_x = 0.0;
public:
    ros::NodeHandle node_handle;
    ros::Publisher publisher;

    void callback(turtlesim::Pose pose) {
        auto command = geometry_msgs::Twist();
        command.linear.x = 5.0;
        command.angular.z = 0.0;

        if (pose.x > 9.0 || pose.x < 2.0 || pose.y > 9.0 || pose.y < 2.0) {
            command.linear.x = 1.0;
            command.angular.z = 1.4;
        }

        if (pose.x >= 5.5 && this->previous_x < 5.5) {
            ROS_INFO("Set color to red!");
            call_set_pen_service(this->node_handle, 255, 0, 0, 3, 0);
        } else if (pose.x < 5.5 && this->previous_x >= 5.5) {
            ROS_INFO("Set color to green!");
            call_set_pen_service(this->node_handle, 0, 255, 0, 3, 0);
        }

        this->previous_x = pose.x;

        this->publisher.publish(command);
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_turtle_controller");
    auto node_handle = ros::NodeHandle();

    ros::service::waitForService(SERVICE_NAME);

    auto publisher = node_handle.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    auto node = Node();
    node.node_handle = node_handle;
    node.publisher = publisher;

    // Save to a variable, even though we don't use it.
    // This is due to C++ RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with Rust.
    auto subscriber = node_handle.subscribe<turtlesim::Pose>("/turtle1/pose", 10, &Node::callback, &node);

    ROS_INFO("Node has been started");

    ros::spin();
}
