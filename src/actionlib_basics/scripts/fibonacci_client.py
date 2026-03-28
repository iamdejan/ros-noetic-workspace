#!/usr/bin/env python3

import rospy
import actionlib
import actionlib_basics.msg


def main():
    rospy.init_node("fibonacci_client")
    action_client = actionlib.SimpleActionClient("fibonacci", actionlib_basics.msg.FibonacciAction)

    rospy.loginfo("Waiting for action server to start...")
    action_client.wait_for_server()

    rospy.loginfo("Action server started, sending goal.")
    goal = actionlib_basics.msg.FibonacciGoal()
    goal.order = 20
    action_client.send_goal(goal)

    finished_before_timeout = action_client.wait_for_result(rospy.Duration(40.0))
    if finished_before_timeout:
        state = action_client.get_state()
        rospy.loginfo(f"Action finished: {state}")

        result: actionlib_basics.msg.FibonacciResult = action_client.get_result()
        sequence = result.sequence
        i = 0
        for element in sequence:
            rospy.loginfo(f"fibonacci({i}) = {element}")
            i += 1
    else:
        rospy.logwarn("Action did not finish before the time out.")


if __name__ == "__main__":
    main()
