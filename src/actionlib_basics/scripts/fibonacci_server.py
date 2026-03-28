#!/usr/bin/env python3

import rospy
import actionlib
import actionlib_basics.msg


class FibonacciAction(object):
    def __init__(self, name: str):
        self.feedback = actionlib_basics.msg.FibonacciFeedback()
        self.result = actionlib_basics.msg.FibonacciResult()

        self.action_name = name
        self.action_server = actionlib.SimpleActionServer(
            self.action_name,
            actionlib_basics.msg.FibonacciAction,
            execute_cb=self.execute_callback,
            auto_start=False,
        )

        self.action_server.start()

    def execute_callback(self, goal: actionlib_basics.msg.FibonacciGoal) -> None:
        rate = rospy.Rate(1)
        success = True

        # append the seeds for Fibonacci sequence
        self.feedback.sequence.clear()
        self.feedback.sequence.append(0)
        self.feedback.sequence.append(1)

        rospy.loginfo(f"{self.action_name}: Executing, creating Fibonacci sequence of order {goal.order} with seeds {self.feedback.sequence[0]}, {self.feedback.sequence[1]}")

        # start executing the action
        for i in range(2, goal.order + 1):
            if self.action_server.is_preempt_requested() or rospy.is_shutdown():
                rospy.loginfo(f"{self.action_name}: Preempted")
                self.action_server.set_preempted()
                success = False
                break

            self.feedback.sequence.append(self.feedback.sequence[i - 1] + self.feedback.sequence[i - 2])

            # publish the feedback
            self.action_server.publish_feedback(self.feedback)

            rate.sleep()

        if success:
            self.result.sequence = self.feedback.sequence;
            rospy.loginfo(f"{self.action_name}: Succeeded")
            self.action_server.set_succeeded(self.result)


def main():
    rospy.init_node("fibonacci_server")

    FibonacciAction("fibonacci")
    rospy.loginfo("Node has been started")
    rospy.spin()


if __name__ == "__main__":
    main()
