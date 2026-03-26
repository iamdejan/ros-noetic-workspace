use rosrust::Publisher;
use rosrust_msg::geometry_msgs;
use rosrust_msg::turtlesim;

const NODE_NAME: &str = "rosrust_draw_square";
const THETA_TARGET: [f64; 4] = [0.0, 90_f64.to_radians(),-180_f64.to_radians(), (270.0_f64-360.0_f64).to_radians()];

fn callback(
    pose: &turtlesim::Pose,
    publisher: &Publisher<geometry_msgs::Twist>
) {
    let mut command = geometry_msgs::Twist::default();
    let theta_target = THETA_TARGET[0];
    _ = pose;
    _ = theta_target;

    publisher.send(command).unwrap();
}

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);

    let publisher = rosrust::publish::<geometry_msgs::Twist>("/turtle1/cmd_vel", 10).unwrap();

    // Save to a variable, even though we don't use it.
    // This is due to Rust RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with C++.
    let _subscriber = rosrust::subscribe("/turtle1/pose", 10, move |pose: turtlesim::Pose| {
        callback(&pose, &publisher);
    })
    .unwrap();

    rosrust::ros_info!("Node has been started");

    rosrust::spin();
}
