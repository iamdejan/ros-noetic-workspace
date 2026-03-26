use std::sync::Mutex;

use rosrust::Publisher;
use rosrust_msg::geometry_msgs;
use rosrust_msg::turtlesim;

const NODE_NAME: &str = "rosrust_draw_square";

#[derive(PartialEq, Eq)]
enum State {
    Still,
    Forward,
    TurnLeft,
}

struct Node {
    publisher: Publisher<geometry_msgs::Twist>,
    current_state: State,
    current_target_index: usize,
    x_limits: [f64; 2],
    y_limits: [f64; 2],
}

impl Node {
    const DELTA: f64 = 1e-2;
    const THETA_TARGETS: [f64; 4] = [
        0.0,
        90_f64.to_radians(),
        -180_f64.to_radians(),
        (270.0_f64 - 360.0_f64).to_radians(),
    ];
    const FORWARD_SPEED: f64 = 4.0;
    const TURN_SPEED: f64 = 0.2;
    const UPDATE_BY_DELTA_MULTIPLIER: f64 = 0.0;

    fn new(publisher: Publisher<geometry_msgs::Twist>) -> Self {
        return Self {
            publisher,
            current_state: State::Still,
            current_target_index: 0,
            x_limits: [2.0, 9.0],
            y_limits: [2.0, 9.0],
        };
    }

    fn need_to_turn_left(&self, pose: &turtlesim::Pose) -> bool {
        if (f64::from(pose.theta) - Self::THETA_TARGETS[0]).abs() <= Self::DELTA
            || (f64::from(pose.theta) - Self::THETA_TARGETS[2]).abs() <= Self::DELTA
        {
            return f64::from(pose.x) < self.x_limits[0] || f64::from(pose.x) > self.x_limits[1];
        }

        if (f64::from(pose.theta) - Self::THETA_TARGETS[1]).abs() <= Self::DELTA
            || (f64::from(pose.theta) - Self::THETA_TARGETS[3]).abs() <= Self::DELTA
        {
            return f64::from(pose.y) < self.y_limits[0] || f64::from(pose.y) > self.y_limits[1];
        }

        return false;
    }

    fn update_limits(&mut self, pose: &turtlesim::Pose) {
        self.x_limits[0] = self.x_limits[0]
            .min(f64::from(pose.x) - Self::UPDATE_BY_DELTA_MULTIPLIER * Self::DELTA);
        self.x_limits[1] = self.x_limits[1]
            .max(f64::from(pose.x) + Self::UPDATE_BY_DELTA_MULTIPLIER * Self::DELTA);

        self.y_limits[0] = self.y_limits[0]
            .min(f64::from(pose.y) - Self::UPDATE_BY_DELTA_MULTIPLIER * Self::DELTA);
        self.y_limits[1] = self.y_limits[1]
            .max(f64::from(pose.y) + Self::UPDATE_BY_DELTA_MULTIPLIER * Self::DELTA);
    }

    fn callback(&mut self, pose: &turtlesim::Pose) {
        let mut command = geometry_msgs::Twist::default();
        let theta_target = Self::THETA_TARGETS[self.current_target_index];

        if self.current_state == State::Still
            && (f64::from(pose.theta) - theta_target).abs() <= Self::DELTA
        {
            command.linear.x = Self::FORWARD_SPEED;
            command.angular.z = 0.0;
            self.current_state = State::Forward;
        } else if self.current_state == State::Forward && self.need_to_turn_left(pose) {
            command.linear.x = 0.0;
            self.current_state = State::Still;
            self.current_target_index = (self.current_target_index + 1) % Self::THETA_TARGETS.len();
        } else if self.current_state == State::Still
            && (f64::from(pose.theta) - theta_target).abs() > Self::DELTA
        {
            self.update_limits(pose);

            command.angular.z = Self::TURN_SPEED;
            self.current_state = State::TurnLeft;
        } else if self.current_state == State::TurnLeft
            && (f64::from(pose.theta) - theta_target).abs() <= Self::DELTA
        {
            command.linear.z = 0.0;
            self.current_state = State::Still;
        } else if self.current_state == State::Forward {
            command.linear.x = Self::FORWARD_SPEED;
        } else if self.current_state == State::TurnLeft {
            command.angular.z = Self::TURN_SPEED;
        }

        self.publisher.send(command).unwrap();
    }
}

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);

    let publisher = rosrust::publish::<geometry_msgs::Twist>("/turtle1/cmd_vel", 10).unwrap();

    // This works because there is only one owner of node_mutex ever exists, which is the closure.
    // If node_mutex is shared between multiple threads, we need to wrap node_mutex with Arc.
    let node_mutex: Mutex<Node> = Mutex::new(Node::new(publisher));

    // Save to a variable, even though we don't use it.
    // This is due to Rust RAII (Resource Acquisition Is Initialization) mechanism,
    // which automatically destructs the resource if it's not save to a variable.
    // By saving to a variable, the resource will not be destroyed until
    // the variable goes out of scope.
    // Similar mechanism happens with C++.
    let _subscriber = rosrust::subscribe("/turtle1/pose", 10, move |pose: turtlesim::Pose| {
        let () = node_mutex
            .lock()
            .map(|mut node| {
                node.callback(&pose);
                return;
            })
            .unwrap();
    })
    .unwrap();

    rosrust::ros_info!("Node has been started");

    rosrust::spin();
}
