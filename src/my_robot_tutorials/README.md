# My Robot Tutorials

This package is to demonstrate the capability of ROS other than controlling turtle robot. This includes:
- anonymous node; and
- Creating and executing simple service in Python and C++.

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
