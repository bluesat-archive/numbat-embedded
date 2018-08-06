project(right_locomotion)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm servo tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES right_locomotion
        CATKIN_DEPENDS ti pwm servo tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES right_locomotion.cpp)
add_module(right_locomotion 3 1 0 "${CPP_FILES}")
target_link_libraries(right_locomotion pwm servo m)
generate_msgs(std_msgs Float64)
depend_msgs(right_locomotion std_msgs Float64)


