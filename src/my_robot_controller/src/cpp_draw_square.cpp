#include <array>
#include <cmath>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>

enum class State {
    STILL,
    FORWARD,
    TURN_LEFT
};

double radians(double degrees) {
    return degrees * M_PI / 180.0;
}

class Node {
private:
    State current_state;
    int current_target_index;

    inline static const double DELTA = 1e-2;
    inline static const std::array<double, 4> THETA_TARGETS = {0.0, radians(90), radians(180-360), radians(270-360)};
    inline static const double FORWARD_SPEED = 4.0;
    inline static const double TURN_SPEED = 0.2;
    inline static const double UPDATE_BY_DELTA_MULTIPLIER = 0.0;

    double x_limits[2] = {2.0, 9.0};
    double y_limits[2] = {2.0, 9.0};

    bool needToTurnLeft(turtlesim::Pose pose) {
        if (std::abs(pose.theta - THETA_TARGETS[0]) <= DELTA || std::abs(pose.theta - THETA_TARGETS[2]) <= DELTA) {
            return pose.x < x_limits[0] || pose.x > x_limits[1];
	    }

        if (std::abs(pose.theta - THETA_TARGETS[1]) <= DELTA || std::abs(pose.theta - THETA_TARGETS[3]) <= DELTA) {
            return pose.y < y_limits[0] || pose.y > y_limits[1];
        }

	    return false;
    }

    void updateLimits(turtlesim::Pose pose) {
        this->x_limits[0] = std::min(x_limits[0], pose.x - UPDATE_BY_DELTA_MULTIPLIER * DELTA);
        this->x_limits[1] = std::max(x_limits[1], pose.x + UPDATE_BY_DELTA_MULTIPLIER * DELTA);

        this->y_limits[0] = std::min(y_limits[0], pose.y - UPDATE_BY_DELTA_MULTIPLIER * DELTA);
        this->y_limits[1] = std::max(y_limits[1], pose.y + UPDATE_BY_DELTA_MULTIPLIER * DELTA);
    }
public:
    ros::NodeHandle nh;
    ros::Publisher publisher;

    void callback(turtlesim::Pose pose) {
        auto command = geometry_msgs::Twist();
        auto theta_target = THETA_TARGETS[this->current_target_index];

        if (current_state == State::STILL && std::abs(pose.theta - theta_target) <= DELTA) {
            command.linear.x = FORWARD_SPEED;
            command.angular.z = 0.0;
            this->current_state = State::FORWARD;
        } else if (current_state == State::FORWARD && needToTurnLeft(pose)) {
            command.linear.x = 0.0;
            this->current_state = State::STILL;
            this->current_target_index = (this->current_target_index + 1) % THETA_TARGETS.size();
        } else if (current_state == State::STILL && std::abs(pose.theta - theta_target) > DELTA) {
            updateLimits(pose);

            command.angular.z = TURN_SPEED;
            this->current_state = State::TURN_LEFT;
        } else if (current_state == State::TURN_LEFT && std::abs(pose.theta - theta_target) <= DELTA) {
            command.linear.z = 0.0;
            this->current_state = State::STILL;
        } else if (current_state == State::FORWARD) {
            command.linear.x = FORWARD_SPEED;
        } else if (current_state == State::TURN_LEFT) {
            command.angular.z = TURN_SPEED;
        }

        this->publisher.publish(command);
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_draw_square");
    auto nh = ros::NodeHandle();

    auto publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    auto node = Node();
    node.nh = nh;
    node.publisher = publisher;

    // Save to a variable, even though we don't use it.
    // This is due to C++ RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with Rust.
    auto subscriber = nh.subscribe<turtlesim::Pose>("/turtle1/pose", 10, &Node::callback, &node);

    ROS_INFO("Node has been started");

    ros::spin();
}
