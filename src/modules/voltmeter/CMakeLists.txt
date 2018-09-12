project(voltmeter)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti adc tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES voltmeter
        CATKIN_DEPENDS ti adc tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

#TODO: add all your module cpp files to this list
set(CPP_FILES voltmeter.cpp can_wait_task.cpp)
add_module(voltmeter 6 1 0 "${CPP_FILES}")
target_link_libraries(voltmeter adc m)

generate_msgs(std_msgs Float64)
depend_msgs(voltmeter std_msgs Float64)

