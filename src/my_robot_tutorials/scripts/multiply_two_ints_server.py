#!/usr/bin/env python3

import rospy
from rospy_tutorials.srv import AddTwoInts, AddTwoIntsRequest, AddTwoIntsResponse

def multiply(request: AddTwoIntsRequest) -> AddTwoIntsResponse:
    response = AddTwoIntsResponse()
    response.sum = request.a * request.b
    return response


def main():
    rospy.init_node("multiply_two_ints_server")
    s = rospy.Service("multiply_two_ints", AddTwoInts, multiply)
    rospy.loginfo("Server has been started. Ready to multiply two ints.")
    rospy.spin()


if __name__ == "__main__":
    main()
