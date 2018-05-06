#cmake_minimum_required(VERSION 3.6)
#project(ros_echronos)

set(CMAKE_VERBOSE_MAKEFILE 1)

#add_definitions(-DPART=TM4C123GH6PM)
#add_definitions(-DTARGET_IS_TM4C123_RB1)

# TODO: include ti_makedefs

set(ROS_ECHRONOS_DIR ../../libs/ros_echronos/)
set(ROS_ARCH_DIR ${ROS_ECHRONOS_DIR}/arch/tm4c)
set(ROS_INCLUDE_DIR ${ROS_ECHRONOS_DIR}/include)

FILE(GLOB ros_files ${ROS_ECHRONOS_DIR}/arch/tm4c/*.cpp ${ROS_ECHRONOS_DIR}/*.cpp )
FILE(GLOB ros_include_files ${ROS_INCLUDE_DIR}/*.hpp ${ROS_INCLUDE_DIR}/templates/*.hpp )



function(build_ros_echronos echronos_build_dir module_name echronos_target node_id serial_on)
    set(ROS_BUILD_DIR ${echronos_build_dir}/lib/ros-echronos)
#    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Dgcc -DROS_NODE_ID=${node_id} -DROS_INFO_SERIAL=${serial_on}")
    message(STATUS "ROS FILES ${ros_files}")
    target_sources(${module_name}
        PRIVATE
            ${ros_files}
        PUBLIC
            ${ros_include_files}
    )
#    add_library(ros_echronos_${module_name} STATIC ${ros_files})
    target_include_directories(
            ${module_name} PUBLIC
            ${ROS_ECHRONOS_DIR}/include
            ${LIB_DIR}/tlsf #TODO: make this its own file
            ${echronos_build_dir}
            ${CMAKE_INCLUDE_PATH}
    )
    set_target_properties(${module_name} PROPERTIES COMPILE_FLAGS "-DROS_NODE_ID=${node_id} -DROS_INFO_SERIAL=${serial_on}")
    add_dependencies(${module_name} ${echronos_target})
    target_link_libraries(${module_name} driverlib utils tlsf)
#    add_dependencies(ros_echronos_${module_name} ${ROS_BUILD_DIR})
endfunction()