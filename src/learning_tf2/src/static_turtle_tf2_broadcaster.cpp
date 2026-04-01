#include <cstdio>

#include <ros/ros.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2/LinearMath/Quaternion.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "my_static_tf2_broadcaster");
    if (argc != 8) {
        ROS_ERROR("Invalid number of parameters!\nUsage: static_turtle_tf2_broadcaster child_frame_name x y z roll pitch yaw");
        return -1;
    }

    if (strcmp(argv[1], "world") == 0) {
        ROS_ERROR("Your static turtle name cannot be 'world'");
        return -1;
    }

    auto static_turtle_name = std::string(argv[1]);

    // Create a StaticTransformBroadcaster object that
    // we will use to send the transformations.
    auto static_broadcaster = tf2_ros::StaticTransformBroadcaster();

    // Create a TransformStamped object which will be
    // the message that will be sent over once it is filled in.
    auto static_transform_stamped = geometry_msgs::TransformStamped();

    // Initialize the TransformStamped object with some data.
    static_transform_stamped.header.stamp = ros::Time::now();
    static_transform_stamped.header.frame_id = "world";
    static_transform_stamped.child_frame_id = static_turtle_name;
    static_transform_stamped.transform.translation.x = atof(argv[2]);
    static_transform_stamped.transform.translation.y = atof(argv[3]);
    static_transform_stamped.transform.translation.z = atof(argv[4]);
    auto quat = tf2::Quaternion();
    quat.setRPY(atof(argv[5]), atof(argv[6]), atof(argv[7]));

    // Send the transform using the StaticTransformBroadcaster sendTransform function.
    static_transform_stamped.transform.rotation.x = quat.x();
    static_transform_stamped.transform.rotation.y = quat.y();
    static_transform_stamped.transform.rotation.z = quat.z();
    static_transform_stamped.transform.rotation.w = quat.w();
    static_broadcaster.sendTransform(static_transform_stamped);
    ROS_INFO("Spinning until killed publishing %s to world", static_turtle_name.c_str());

    ros::spin();
}
