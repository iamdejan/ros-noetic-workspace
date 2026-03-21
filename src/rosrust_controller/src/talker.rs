const NODE_NAME: &'static str = "rosrust_talker";

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);
    rosrust::ros_info!("Node has been started");

    let publisher =
        rosrust::publish::<rosrust_msg::std_msgs::String>("/robot_news_radio", 10).unwrap();
    publisher.wait_for_subscribers(None).unwrap();

    let rate = rosrust::rate(5.0);
    while rosrust::is_ok() {
        let message = rosrust_msg::std_msgs::String {
            data: format!("Hi, this is {NODE_NAME} node speaking!"),
        };
        rosrust::ros_info!("Sending message...");
        publisher.send(message).unwrap();

        rate.sleep();
    }

    rosrust::spin();
}
