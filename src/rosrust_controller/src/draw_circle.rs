const NODE_NAME: &'static str = "rosrust_rusty_talker";

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);
    rosrust::ros_info!("Node has been started");

    let publisher =
        rosrust::publish::<rosrust_msg::geometry_msgs::Twist>("/turtle1/cmd_vel", 10).unwrap();
    let rate = rosrust::rate(2.0);
    while rosrust::is_ok() {
        let mut message = rosrust_msg::geometry_msgs::Twist::default();
        message.linear.x = 2.0;
        message.angular.z = 1.0;
        publisher.send(message).unwrap();

        rate.sleep();
    }
}
