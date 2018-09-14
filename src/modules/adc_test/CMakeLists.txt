project(adc_test)
cmake_minimum_required(VERSION 3.5)

find_package(catkin REQUIRED
        COMPONENTS ti adc tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES adc_test
        CATKIN_DEPENDS ti adc tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

#set(CPP_FILES adc_test.cpp can_wait_task.cpp)
set(CPP_FILES adc_test.cpp)
add_module(adc_test 4 1 0 "${CPP_FILES}")
target_link_libraries(adc_test adc m)


