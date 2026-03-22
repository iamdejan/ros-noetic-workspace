use rosrust_msg::turtlesim;

const NODE_NAME: &str = "rosrust_pose_subscriber";

fn pose_callback(pose: &turtlesim::Pose) {
    let log = format!("({}, {})", pose.x, pose.y);
    rosrust::ros_info!("{log}");
}

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);

    // Save to a variable, even though we don't use it.
    // This is due to Rust RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with C++.
    let _subscriber = rosrust::subscribe("/turtle1/pose", 10, move |pose: turtlesim::Pose| {
        pose_callback(&pose);
    })
    .unwrap();

    rosrust::ros_info!("Node has been started");

    rosrust::spin();
}
