project(pid)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES pid
        CATKIN_DEPENDS ti
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(../../../build-tools/build_env.cmake)

add_library(pid STATIC pid.cpp)
target_include_directories(pid PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)