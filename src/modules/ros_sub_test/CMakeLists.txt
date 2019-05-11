project(ros_sub_test)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm servo tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES ros_sub_test
        CATKIN_DEPENDS ti pwm servo tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES ros_sub_test.cpp)
add_module(ros_sub_test 4 1 0 "${CPP_FILES}")
target_link_libraries(ros_sub_test pwm servo m)
generate_msgs(owr_messages pwm)
depend_msgs(ros_sub_test owr_messages pwm)

generate_msgs(std_msgs String)
depend_msgs(ros_sub_test std_msgs String)

