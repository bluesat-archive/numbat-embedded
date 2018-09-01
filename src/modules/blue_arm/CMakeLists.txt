project(blue_arm)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm servo tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES blue_arm
        CATKIN_DEPENDS ti pwm servo tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

#TODO: add all your module cpp files to this list
set(CPP_FILES blue_arm.cpp can_wait_task.cpp)
add_module(blue_arm 4 1 0 "${CPP_FILES}")
target_link_libraries(blue_arm pwm servo m)

generate_msgs(std_msgs Float64)  
depend_msgs(blue_arm std_msgs Float64)

