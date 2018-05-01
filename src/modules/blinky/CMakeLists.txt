project(blinky)
cmake_minimum_required(VERSION 3.6)

find_package(catkin REQUIRED
        COMPONENTS ti)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES blinky
        CATKIN_DEPENDS ti
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

add_module(blinky blinky.cpp)


