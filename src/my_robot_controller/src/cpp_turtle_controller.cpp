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
    ros::NodeHandle nh;
    ros::Publisher publisher;

    void callback(turtlesim::Pose pose) {
        auto cmd = geometry_msgs::Twist();
        cmd.linear.x = 5.0;
        cmd.angular.z = 0.0;

        if (pose.x > 9.0 || pose.x < 2.0 || pose.y > 9.0 || pose.y < 2.0) {
            cmd.linear.x = 1.0;
            cmd.angular.z = 1.4;
        }

        if (pose.x >= 5.5 && this->previous_x < 5.5) {
            ROS_INFO("Set color to red!");
            call_set_pen_service(this->nh, 255, 0, 0, 3, 0);
        } else if (pose.x < 5.5 && this->previous_x >= 5.5) {
            ROS_INFO("Set color to green!");
            call_set_pen_service(this->nh, 0, 255, 0, 3, 0);
        }

        this->previous_x = pose.x;

        this->publisher.publish(cmd);
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_turtle_controller");
    auto nh = ros::NodeHandle();

    ros::service::waitForService(SERVICE_NAME);

    auto publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    auto node = Node();
    node.nh = nh;
    node.publisher = publisher;
    auto sub = nh.subscribe<turtlesim::Pose>("/turtle1/pose", 10, &Node::callback, &node);

    ROS_INFO("Node has been started");

    ros::spin();
}
