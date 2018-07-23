project(i2c)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti)

catkin_package(
        INCLUDE_DIRS ./ ./science-mod
        LIBRARIES i2c
        CATKIN_DEPENDS ti
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(../../../build-tools/build_env.cmake)

FILE(GLOB science-mod_files science-mod/*.cpp)
add_library(i2c STATIC i2c.c ${science-mod_files})
target_include_directories(i2c PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
	./science-mod      
        ${catkin_INCLUDE_DIRS}
)
