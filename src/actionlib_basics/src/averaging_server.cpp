#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <actionlib/server/simple_action_server.h>
#include <actionlib_basics/AveragingAction.h>

class AveragingAction {
private:
    ros::NodeHandle nh = ros::NodeHandle();

    actionlib::SimpleActionServer<actionlib_basics::AveragingAction> action_server;

    std::string action_name;

    actionlib_basics::AveragingFeedback feedback;
    actionlib_basics::AveragingResult result;

    int data_count, goal;
    double sum, sum_of_squared;

    ros::Subscriber subscriber;
public:
    AveragingAction(std::string name) :
        action_server(nh, name, false),
        action_name(name) {
            subscriber = nh.subscribe("/random_number", 10, &AveragingAction::analysisCallback, this);

            action_server.registerGoalCallback(boost::bind(&AveragingAction::goalCallback, this));
            action_server.registerPreemptCallback(boost::bind(&AveragingAction::preemptCallback, this));
            action_server.start();
        }

    void goalCallback() {
        data_count = 0;
        sum = 0;
        sum_of_squared = 0;

        goal = action_server.acceptNewGoal()->samples;
    }

    void preemptCallback() {
        ROS_INFO("%s: Preempted", action_name.c_str());
        action_server.setPreempted();
    }

    void analysisCallback(const std_msgs::Float64::ConstPtr& message) {
        ROS_INFO("Incoming message: %lf", message->data);

        // make sure that action server is active
        if (!action_server.isActive()) {
            ROS_WARN("%s: not active when calling analysisCallback", action_name.c_str());
            return;
        }

        data_count += 1;
        feedback.sample = data_count;
        feedback.data = message->data;

        // compute the standard deviation and mean of this data
        sum += message->data;
        feedback.mean = sum * 1.0 / data_count;
        sum_of_squared += pow(message->data, 2);
        feedback.standard_deviation = sqrt(fabs((sum_of_squared * 1.0 / data_count) - pow(feedback.mean, 2)));
        action_server.publishFeedback(feedback);

        if (data_count >= goal) {
            result.mean = feedback.mean;
            result.standard_deviation = feedback.standard_deviation;

            if (result.mean < 5.0) {
                ROS_WARN("%s: Aborted", action_name.c_str());
                action_server.setAborted(result);
            } else {
                ROS_INFO("%s: Succeeded", action_name.c_str());
                action_server.setSucceeded(result);
            }
        }
    }
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "averaging_server");

    auto averaging = AveragingAction("averaging");

    ROS_INFO("Node has been started");

    ros::spin();
}
