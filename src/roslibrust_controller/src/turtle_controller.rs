roslibrust_codegen_macro::find_and_generate_ros_messages!();

use roslibrust::{Publish, Service, ServiceProvider, Subscribe, TopicProvider};
use tokio::signal;
use tokio_util::sync::CancellationToken;

const NODE_NAME: &str = "roslibrust_turtle_controller";
const SERVICE_NAME: &str = "/turtle1/set_pen";

async fn call_set_pen_service<T: ServiceProvider>(
    client: &T::ServiceClient<turtlesim::SetPen>,
    r: u8,
    g: u8,
    b: u8,
    width: u8,
    off: u8,
) -> roslibrust::Result<turtlesim::SetPenResponse> {
    let command = turtlesim::SetPenRequest {
        r,
        g,
        b,
        width,
        off,
    };
    return client.call(&command).await;
}

async fn do_work<T: TopicProvider + ServiceProvider>(ros: T) -> roslibrust::Result<()> {
    let client = ros
        .service_client::<turtlesim::SetPen>(SERVICE_NAME)
        .await?;
    let mut subscriber = ros.subscribe::<turtlesim::Pose>("/turtle1/pose").await?;
    let publisher = ros
        .advertise::<geometry_msgs::Twist>("/turtle1/cmd_vel")
        .await?;
    println!("Node has been started");

    let mut previous_x: f32 = 0.0;
    while let Ok(pose) = subscriber.next().await {
        let mut command = geometry_msgs::Twist {
            linear: geometry_msgs::Vector3 {
                x: 5.0,
                y: 0.0,
                z: 0.0,
            },
            angular: geometry_msgs::Vector3 {
                x: 0.0,
                y: 0.0,
                z: 0.0,
            },
        };

        if pose.x < 2.0 || pose.x > 9.0 || pose.y < 2.0 || pose.y > 9.0 {
            command.linear.x = 1.0;
            command.angular.z = 1.4;
        }

        if pose.x >= 5.5 && previous_x < 5.5 {
            call_set_pen_service::<T>(&client, 255, 0, 0, 3, 0).await?;
        } else if pose.x < 5.5 && previous_x >= 5.5 {
            call_set_pen_service::<T>(&client, 0, 255, 0, 3, 0).await?;
        }

        previous_x = pose.x;

        publisher.publish(&command).await?;
    }

    return Ok(());
}

async fn relay<T: TopicProvider + ServiceProvider>(nh: T) -> roslibrust::Result<()> {
    let cancel = CancellationToken::new();
    let cloned_cancel = cancel.clone();

    // Spawn a task to listen for the Ctrl+C signal
    tokio::spawn(async move {
        signal::ctrl_c().await.expect("failed to handle Ctrl + C");
        println!("🛑 Ctrl+C pressed");
        cloned_cancel.cancel(); // Signal all other tasks to cancel
    });

    tokio::select! {
        res = do_work(nh) => {
            if let Err(e) = res {
                eprintln!("Worker stopped with error: {e:?}");
            }
        },
        () = cancel.cancelled() => {
            println!("Gracefully shutting down...");
        },
    };

    return Ok(());
}

#[tokio::main]
async fn main() -> roslibrust::Result<()> {
    let ros = roslibrust::ros1::NodeHandle::new("http://localhost:11311", NODE_NAME).await?;
    relay(ros).await?;

    return Ok(());
}
