#!/usr/bin/env python3

import rospy
from my_robot_tutorials.srv import MultiplyTwoNumbers, MultiplyTwoNumbersRequest, MultiplyTwoNumbersResponse

SERVICE_NAME = "multiply_two_numbers"

def multiply(request: MultiplyTwoNumbersRequest) -> MultiplyTwoNumbersResponse:
    response = MultiplyTwoNumbersResponse()
    response.result = request.a * request.b
    return response


def main():
    rospy.init_node(f"{SERVICE_NAME}_server")
    rospy.Service(SERVICE_NAME, MultiplyTwoNumbers, multiply)
    rospy.loginfo("Server has been started.")
    rospy.loginfo("Ready to multiply two numbers.")
    rospy.spin()


if __name__ == "__main__":
    main()
