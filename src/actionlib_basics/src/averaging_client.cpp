#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib_basics/AveragingAction.h>

void spinThread() {
    ros::spin();
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "averaging_client");

    auto action_client = actionlib::SimpleActionClient<actionlib_basics::AveragingAction>("averaging", true);
    auto spin_thread = boost::thread(&spinThread);

    ROS_INFO("Waiting for action server to start...");
    action_client.waitForServer();

    ROS_INFO("Action server started, sending goal.");

    auto goal = actionlib_basics::AveragingGoal();
    goal.samples = 100;
    action_client.sendGoal(goal);

    ROS_INFO("Goal sent, now waiting for results.");

    // wait for action to return
    bool finished_before_timeout = action_client.waitForResult(ros::Duration(40.0));
    if (finished_before_timeout) {
        auto state = action_client.getState();
        ROS_INFO("Action finished: %s", state.toString().c_str());

        auto result = action_client.getResult();
        ROS_INFO("Mean / average: %lf", result->mean);
        ROS_INFO("Standard deviation: %lf", result->standard_deviation);
    } else {
        ROS_WARN("Action did not finish before the time out.");
    }

    ros::shutdown();
    spin_thread.join();
}
