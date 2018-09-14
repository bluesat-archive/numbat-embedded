project(science_test)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti tlsf i2c adc servo pwm)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES science_test
        CATKIN_DEPENDS ti tlsf i2c adc servo pwm
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

add_module(science_test 6 1 0 science_test.cpp)
link_science_library(science_test)
target_link_libraries(science_test i2c adc servo pwm)

