project(lidar_tilt)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm servo tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES lidar_tilt
        CATKIN_DEPENDS ti pwm servo tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)
set(CPP_FILES lidar_tilt.cpp can_wait_task.cpp)
add_module(lidar_tilt 9 1 0 "${CPP_FILES}")
target_link_libraries(lidar_tilt pwm servo m)
generate_msgs(std_msgs Float64)
#generate_msgs(std_msgs Header)
#generate_msgs(sensor_msgs JointState)
depend_msgs(lidar_tilt std_msgs Float64)
#depend_msgs(lidar_tilt std_msgs Header)
#depend_msgs(lidar_tilt sensor_msgs JointState)
