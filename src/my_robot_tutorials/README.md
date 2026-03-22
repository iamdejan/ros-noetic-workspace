# My Robot Tutorials

This package is to demonstrate the capability of ROS other than controlling turtle robot. This includes:
- implementing anonymous node; and
- creating service and client in Python and C++.

Complete tutorials for ROS Noetic can be seen [here](https://wiki.ros.org/ROS/Tutorials).

## Anonymous Mode

### Python

Enable anonymous node with `anonymous=True` when initializing the node.

Example:
```py
rospy.init_node("radio_transmitter", anonymous=True)
```

### C++

Enable anonymous node with `ros::InitOption::AnonymousName` when initializing the node.

Example:
```cpp
ros::init(argc, argv, "radio_transmitter", ros::InitOption::AnonymousName);
```

## Create Service and Client

References:
- [How to Create a Service in ROS Noetic](https://automaticaddison.com/how-to-create-a-service-in-ros-noetic/)
- [Writing a Simple Service and Client (Python)](https://wiki.ros.org/ROS/Tutorials/WritingServiceClient%28python%29)
- [Writing a Simple Service and Client (C++)](https://wiki.ros.org/ROS/Tutorials/WritingServiceClient%28c%2B%2B%29)
