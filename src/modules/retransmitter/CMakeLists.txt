project(retransmitter)
cmake_minimum_required(VERSION 3.6)

find_package(catkin REQUIRED
        COMPONENTS ti tlsf)

catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES retransmitter
        CATKIN_DEPENDS ti tlsf
)

include_directories(
        ${CMAKE_INCLUDE_PATH}
        ./
        ${catkin_INCLUDE_DIRS}
)

set(BUILD_TOOLS_DIR ../../../build-tools)
include(${BUILD_TOOLS_DIR}/module_template.cmake)

set(CPP_FILES retransmitter.cpp)
add_module(retransmitter "${CPP_FILES}")


