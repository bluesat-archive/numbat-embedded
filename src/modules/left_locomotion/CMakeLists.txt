project(left_locomotion)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm servo tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES left_locomotion
        CATKIN_DEPENDS ti pwm servo tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES left_locomotion.cpp)
add_module(left_locomotion 2 1 0 "${CPP_FILES}")
target_link_libraries(left_locomotion pwm servo m)

generate_msgs(std_msgs Float64)
depend_msgs(left_locomotion std_msgs Float64)

