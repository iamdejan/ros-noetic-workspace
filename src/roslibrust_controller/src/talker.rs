roslibrust_codegen_macro::find_and_generate_ros_messages!();

use roslibrust::{Publish, TopicProvider};
use std::time::Duration;
use tokio::signal;
use tokio_util::sync::CancellationToken;

const NODE_NAME: &str = "roslibrust_talker";

async fn do_work<T: TopicProvider>(ros: T) -> roslibrust::Result<()> {
    let publisher = ros
        .advertise::<std_msgs::String>("/robot_news_radio")
        .await?;

    loop {
        let message = std_msgs::String {
            data: format!("Hi, this is {NODE_NAME} node speaking!"),
        };
        publisher.publish(&message).await?;

        tokio::time::sleep(Duration::from_secs(1)).await;
    }
}

async fn relay<T: TopicProvider>(nh: T) -> roslibrust::Result<()> {
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
