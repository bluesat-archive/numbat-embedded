project(science)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti tlsf i2c adc servo pwm)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES science
        CATKIN_DEPENDS ti tlsf i2c adc servo pwm
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES science.cpp can_wait_task.cpp)
add_module(science 7 1 0 "${CPP_FILES}")
link_science_library(science)
target_link_libraries(science i2c adc servo pwm)
generate_msgs(std_msgs Int16)
generate_msgs(std_msgs Float64)
generate_msgs(owr_messages science)
depend_msgs(science std_msgs Float64)
depend_msgs(science std_msgs Int16)
depend_msgs(science owr_messages science)
