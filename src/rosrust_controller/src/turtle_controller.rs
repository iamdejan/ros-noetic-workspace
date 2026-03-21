use std::sync::Arc;
use std::sync::Mutex;
use std::time::Duration;

use rosrust_msg::geometry_msgs;
use rosrust_msg::turtlesim;

const NODE_NAME: &'static str = "rosrust_turtle_controller";
const SERVICE_NAME: &'static str = "/turtle1/set_pen";

fn call_set_pen_service(r: u8, g: u8, b: u8, width: u8, off: u8) {
    let client = rosrust::client::<turtlesim::SetPen>(SERVICE_NAME).unwrap();
    let mut command = turtlesim::SetPenReq::default();
    command.r = r;
    command.g = g;
    command.b = b;
    command.width = width;
    command.off = off;
    client.req(&command).unwrap().unwrap();
}

fn main() {
    env_logger::init();
    rosrust::init(NODE_NAME);
    rosrust::wait_for_service(SERVICE_NAME, Some(Duration::from_secs(10))).unwrap();

    let previous_x = Arc::new(Mutex::new(0.0_f32));

    let publisher = rosrust::publish::<geometry_msgs::Twist>("/turtle1/cmd_vel", 10).unwrap();
    let _subscriber = rosrust::subscribe("/turtle1/pose", 10, move |pose: turtlesim::Pose| {
        let mut command = geometry_msgs::Twist::default();
        command.linear.x = 5.0;
        command.angular.z = 0.0;

        if pose.x < 2.0 || pose.x > 9.0 || pose.y > 9.0 || pose.y < 2.0 {
            command.linear.x = 1.0;
            command.angular.z = 1.4;
        }

        let previous_x_val = *previous_x.lock().unwrap();

        if pose.x >= 5.5 && previous_x_val < 5.5 {
            call_set_pen_service(255, 0, 0, 3, 0);
        } else if pose.x < 5.5 && previous_x_val >= 5.5 {
            call_set_pen_service(0, 255, 0, 3, 0);
        }

        *previous_x.lock().unwrap() = pose.x;

        publisher.send(command).unwrap();
    }).unwrap();

    rosrust::ros_info!("Node has been started");

    rosrust::spin();
}
