#include <string>

#include <ros/ros.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <turtlesim/Pose.h>

class Node {
private:
    std::string turtle_name;
public:
    Node(std::string turtle_name) : turtle_name(turtle_name) {}

    void poseCallback(turtlesim::Pose message) {
        // Create a TransformBroadcaster object that
        // we will use to send the transformations.
        static tf2_ros::TransformBroadcaster broadcaster;

        // Create a TransformStamped object that
        // we will use to send the transformations.
        auto transform_stamped = geometry_msgs::TransformStamped();

        // Initialize the transform_stamped object with some data
        transform_stamped.header.stamp = ros::Time::now();
        transform_stamped.header.frame_id = "world";
        transform_stamped.child_frame_id = this->turtle_name;

        // Copy transformation from 3D turtle post to 3D transform
        transform_stamped.transform.translation.x = message.x;
        transform_stamped.transform.translation.y = message.y;
        transform_stamped.transform.translation.z = 0.0;

        // Set roll-pitch-yaw in quaternion for rotation.
        // A quaternion represents rotation using 4 numbers: (x, y, z, w).
        // Advantages:
        // - No gimbal lock
        // - Smooth interpolation (SLERP)
        // - Numerically stable
        // - Efficient computation
        // - Compact representation
        // - Perfect for continuous motion
        auto quaternion = tf2::Quaternion();
        quaternion.setRPY(0, 0, message.theta);
        transform_stamped.transform.rotation.x = quaternion.x();
        transform_stamped.transform.rotation.y = quaternion.y();
        transform_stamped.transform.rotation.z = quaternion.z();
        transform_stamped.transform.rotation.w = quaternion.w();

        // Send the transform
        broadcaster.sendTransform(transform_stamped);
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "my_tf2_broadcaster");

    std::string turtle_name;

    auto private_node = ros::NodeHandle("~");
    if (!private_node.hasParam("turtle")) {
        if (argc != 2) {
            ROS_ERROR("need turtle name as argument");
            return -1;
        }

        turtle_name = argv[1];
    } else {
        private_node.getParam("turtle", turtle_name);
    }

    auto nh = ros::NodeHandle();
    auto node = Node(turtle_name);
    auto subscriber = nh.subscribe(turtle_name+"/pose", 10, &Node::poseCallback, &node);

    ros::spin();
}
