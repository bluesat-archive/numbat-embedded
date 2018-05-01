project(tlsf)
cmake_minimum_required(VERSION 3.6)

find_package(catkin REQUIRED)
catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES tlsf
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(../../../build-tools/build_env.cmake)

add_library(tlsf STATIC tlsf.c)