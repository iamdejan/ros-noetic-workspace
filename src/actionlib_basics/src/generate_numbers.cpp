#include <random>

#include <ros/ros.h>
#include <std_msgs/Float64.h>

const double MEAN = 5.0;
const double STANDARD_DEVIATION = 1.0;
const int PUBLISH_COUNT_LIMIT = 100;

int main(int argc, char **argv) {
    ros::init(argc, argv, "generate_numbers");
    ros::NodeHandle node_handle;

    auto random_device = std::random_device();
    auto generator = std::mt19937(random_device());
    auto normal_distribution = std::normal_distribution<double>(MEAN, STANDARD_DEVIATION);

    auto publisher = node_handle.advertise<std_msgs::Float64>("/random_number", 10);
    auto rate = ros::Rate(20.0);

    // Wait for subscriber to be registered, since we don't want wasted/discarded message.
    // At this stage, action server is guaranteed to be started.
    while (publisher.getNumSubscribers() < 1) {
        rate.sleep();
    }

    int sample_count = 1;
    while (ros::ok()) {
        double number = normal_distribution(generator);
        auto message = std_msgs::Float64();
        message.data = number;
        publisher.publish(message);
        ROS_INFO("[sample %d] Generate number %lf", sample_count, number);

        if (sample_count >= PUBLISH_COUNT_LIMIT) {
            break;
        }

        sample_count += 1;
        rate.sleep();
    }

    ROS_INFO("Finished generating numbers");
}
