project(servo)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti pwm)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES servo
        CATKIN_DEPENDS ti pwm
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(../../../build-tools/build_env.cmake)

add_library(servo STATIC servo.c)
target_include_directories(servo PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)