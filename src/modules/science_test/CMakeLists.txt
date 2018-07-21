project(science_test)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti i2c)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES science_test
        CATKIN_DEPENDS ti i2c
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

add_module(science_test 4 1 0 science_test.cpp)


