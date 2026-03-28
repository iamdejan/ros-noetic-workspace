#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <actionlib_basics/FibonacciAction.h>

int main(int argc, char **argv) {
    ros::init(argc, argv, "fibonacci_client");

    auto action_client = actionlib::SimpleActionClient<actionlib_basics::FibonacciAction>("fibonacci", true);

    ROS_INFO("Waiting for action server to start...");
    action_client.waitForServer();

    ROS_INFO("Action server started, sending goal.");
    auto goal = actionlib_basics::FibonacciGoal();
    goal.order = 20;
    action_client.sendGoal(goal);

    // wait for the action to return
    bool finished_before_timeout = action_client.waitForResult(ros::Duration(40.0));
    if (finished_before_timeout) {
        auto state = action_client.getState();
        ROS_INFO("Action finished: %s", state.toString().c_str());

        auto result = action_client.getResult();
        auto sequence = result->sequence;
        int i = 0;
        for (const auto& element : sequence) {
            ROS_INFO("fibonacci(%i) = %i", i, element);
            i += 1;
        }
    } else {
        ROS_INFO("Action did not finish before the time out.");
    }
}
