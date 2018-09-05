project(adc)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES adc
        CATKIN_DEPENDS ti
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(../../../build-tools/build_env.cmake)

add_library(adc STATIC adc.c)
target_include_directories(adc PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)
