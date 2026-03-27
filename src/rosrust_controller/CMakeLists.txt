cmake_minimum_required(VERSION 2.8.3)
project(rosrust_controller)
find_package(catkin REQUIRED roscpp std_msgs)

catkin_package()

add_custom_target(rorust_controller
   ALL
   COMMENT "Building ${PROJECT_NAME}..."
   COMMAND cargo build --manifest-path ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
)
