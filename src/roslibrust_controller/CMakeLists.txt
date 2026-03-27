cmake_minimum_required(VERSION 3.0.2)
project(roslibrust_controller)
find_package(catkin REQUIRED roscpp std_msgs)
catkin_package()

add_custom_target(${PROJECT_NAME}
   ALL
   COMMENT "Building ${PROJECT_NAME}..."
   COMMAND cargo build --manifest-path ${CMAKE_CURRENT_SOURCE_DIR}/Cargo.toml
)
