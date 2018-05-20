cmake_minimum_required(VERSION 3.6)
project(ros_echronos)


add_definitions(-DPART=TM4C123GH6PM)
add_definitions(-DTARGET_IS_TM4C123_RB1)

# TODO: include ti_makedefs

set(ROS_ECHRONOS_DIR=libs/ros_ecrhonos)
set(ROS_ARCH_DIR=${ROS_ECHRONOS_DIR}/arch/tm4c)

include_directories(
)

FILE(GLOB ros_files libs/ros_echronos/*.cpp libs/ros_echronos/include/*.hpp libs/ros_echronos/arch/tm4c/)



function(build_ros_echronos echronos_build_dir echronos_target)
    set(ROS_BUILD_DIR ${echronos_build_dir}/lib/ros-echronos)
    add_custom_command(
            OUTPUT ${ROS_BUILD_DIR}
            COMMAND mkdir -p ${ROS_BUILD_DIR}
    )
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Dgcc)
    add_library(ros_echronos_${echronos_target} ${ros_files})
    target_include_directories(
            ros_echronos_${echronos_target}
            STATIC
            ${ROS_ECHRONOS_DIR}/include
            ${LIB_DIR}/tlsf #TODO: make this its own file
            ${echronos_build_dir}
            ${CMAKE_INCLUDE_PATH}
    )
    add_dependencies(ros_echronos_${echronos_target} ${echronos_target})
    add_dependencies(ros_echronos_${echronos_target} ${ROS_BUILD_DIR})
endfunction()