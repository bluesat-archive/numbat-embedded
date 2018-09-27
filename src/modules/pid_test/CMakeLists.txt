project(left_locomotion)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES pid_test
        CATKIN_DEPENDS ti pid
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES pid_test.cpp)
add_module(pid_test 9 1 0 "${CPP_FILES}")
target_link_libraries(pid_test pid)