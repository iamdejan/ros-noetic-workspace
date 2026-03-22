use rosrust_msg::turtlesim;

const NODE_NAME: &str = "rosrust_pose_subscriber";

fn pose_callback(pose: &turtlesim::Pose) {
    let log = format!("({}, {})", pose.x, pose.y);
    rosrust::ros_info!("{log}");
}

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);

    let _subscriber = rosrust::subscribe("/turtle1/pose", 10, move |pose: turtlesim::Pose| {
        pose_callback(&pose);
    })
    .unwrap();

    rosrust::ros_info!("Node has been started");

    rosrust::spin();
}
