#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "my_tf2_listener");

    auto nh = ros::NodeHandle();

    // "spawn" service is used to spawn a turtle into the turtlesim window.
    ros::service::waitForService("spawn");
    auto spawner = nh.serviceClient<turtlesim::Spawn>("spawn");
    auto turtle = turtlesim::Spawn();
    turtle.request.x = 4;
    turtle.request.y = 2;
    turtle.request.theta = 0;
    turtle.request.name = "turtle2";
    spawner.call(turtle);

    // Create a Publisher to publish velocity for turtle2.
    auto turtle_velocity_publisher = nh.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 10);

    // Create TransformListener object to retrieve tf2 transformations
    auto tf_buffer = tf2_ros::Buffer();
    auto tf_listener = tf2_ros::TransformListener(tf_buffer);

    auto rate = ros::Rate(10.0);

    while (nh.ok()) {
        auto transform_stamped = geometry_msgs::TransformStamped();
        try {
            // Calculate the transform between 2 frames, from turtle2 -> turtle1.
            transform_stamped = tf_buffer.lookupTransform("turtle2", "turtle1", ros::Time(0));
        } catch (tf2::TransformException &ex) {
            ROS_WARN("%s", ex.what());
            ros::Duration(1.0).sleep();
            continue;
        }

        // Calculate the velocity needed to get to the turtle1.
        auto velocity_message = geometry_msgs::Twist();

        // atan2 measures the angle based on y and x.
        // atan2 reference: https://en.wikipedia.org/wiki/Atan2
        velocity_message.angular.z = 4.0 * atan2(transform_stamped.transform.translation.y, transform_stamped.transform.translation.x);
        velocity_message.linear.x = 0.5 * sqrt(pow(transform_stamped.transform.translation.x, 2) + pow(transform_stamped.transform.translation.y, 2));
        turtle_velocity_publisher.publish(velocity_message);

        rate.sleep();
    }
}
