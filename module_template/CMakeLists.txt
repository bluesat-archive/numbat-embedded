# TODO: change this to your module name (must match package.xml)
project(module_template)
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

#TODO: add all your module cpp files to this list
set(CPP_FILES template_sub.cpp can_wait_task.cpp)
add_module(ros_sub_test 4 1 0 "${CPP_FILES}")
target_link_libraries(ros_sub_test pwm servo m)


