#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <actionlib_basics/FibonacciAction.h>

class FibonacciAction {
protected:
    ros::NodeHandle nh = ros::NodeHandle();

    // NodeHandle instance must be created before this line. Otherwise strange error occurs.
    actionlib::SimpleActionServer<actionlib_basics::FibonacciAction> action_server;

    std::string action_name;

    actionlib_basics::FibonacciFeedback feedback;
    actionlib_basics::FibonacciResult result;
public:
    FibonacciAction(std::string name) :
        action_server(nh, name, boost::bind(&FibonacciAction::executeCallback, this, _1), false),
        action_name(name) {
            action_server.start();
        }
    
    ~FibonacciAction(void) {}

    void executeCallback(const actionlib_basics::FibonacciGoalConstPtr &goal) {
        // helper variables
        auto rate = ros::Rate(1);
        bool success = true;

        // push back the seeds for the Fibonacci sequence
        feedback.sequence.clear();
        feedback.sequence.push_back(0);
        feedback.sequence.push_back(1);

        // publish info to the console for the user
        ROS_INFO("%s: Executing, creating Fibonacci sequence of order %li with seeds %i, %i", action_name.c_str(), goal->order, feedback.sequence[0], feedback.sequence[1]);
    
        // start executing the action
        for (int i = 1; i <= goal->order; i++) {
            if (action_server.isPreemptRequested() || !ros::ok()) {
                ROS_INFO("%s: Preempted", action_name.c_str());
                action_server.setPreempted();
                success = false;
                break;
            }

            feedback.sequence.push_back(feedback.sequence[i] +feedback.sequence[i - 1]);

            action_server.publishFeedback(feedback);

            rate.sleep();
        }

        if (success) {
            result.sequence = feedback.sequence;
            ROS_INFO("%s: Succeeded", action_name.c_str());
            action_server.setSucceeded(result);
        }
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "fibonacci");

    // Previously, I'm creating a FibonacciAction object with auto:
    // ```
    // auto fibonacci = FibonacciAction("fibonacci");
    // ```
    // which tries to make a copy.
    // But FibonacciAction contains a SimpleActionServer member,
    // which internally has boost::recursive_mutex, boost::condition_variable_any, and boost::mutex,
    // all of which are non-copyable (deleted copy constructors).
    // SimpleActionServer is move-only or not copyable-by-default
    // because it manages thread synchronization primitives that shouldn't be copied.
    // To avoid such issue, use direct-initialization. 
    FibonacciAction fibonacci("fibonacci");
    
    ros::spin();
}
