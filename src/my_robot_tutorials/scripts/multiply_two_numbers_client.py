#!/usr/bin/env python3

import sys

import rospy
from my_robot_tutorials.srv import MultiplyTwoNumbers, MultiplyTwoNumbersResponse


SERVICE_NAME = "/multiply_two_numbers"

def main(a: float, b: float):
    rospy.wait_for_service(SERVICE_NAME)
    print(f"Requesting {a} * {b}...")

    try:
        s = rospy.ServiceProxy(SERVICE_NAME, MultiplyTwoNumbers)
        response: MultiplyTwoNumbersResponse = s.call(a, b)
        print(f"{a} * {b} = {response.result}")
    except rospy.ServiceException as e:
        print(f"Service call failed: {e}")


def usage():
    return f"{sys.argv[0]} a b"


if __name__ == "__main__":
    if len(sys.argv) == 3:
        a = float(sys.argv[1])
        b = float(sys.argv[2])
    else:
        print(usage())
        sys.exit(1)

    main(a, b)
