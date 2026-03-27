cmake_minimum_required(VERSION 2.8.3)
project(rosrust_controller)
find_package(catkin REQUIRED roscpp std_msgs)
catkin_package()

set(RUST_PKG_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Command to build the Rust project using Cargo for talker.rs
add_custom_command(OUTPUT ${CATKIN_DEVEL_PREFIX}/lib/${PROJECT_NAME}/talker
  COMMAND cargo build --bin talker
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/target/debug/talker ${CATKIN_DEVEL_PREFIX}/lib/${PROJECT_NAME}/
  DEPENDS ${RUST_PKG_DIR}/src/talker.rs ${RUST_PKG_DIR}/Cargo.toml
  WORKING_DIRECTORY ${RUST_PKG_DIR}
  COMMENT "Building Rust Node"
)
