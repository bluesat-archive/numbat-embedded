#cmake_minimum_required(VERSION 3.5)
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

# define the doxygen target if there isn't one already
# if(NOT TARGET ros-echronos-docs)
#     find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
#     set(DOXYGEN_OUTPUT_DIRECTORY ../../../docs/doxygen)
#     doxygen_add_docs(
#             ros-echronos-docs
#             ${ros_files}
#             ${ros_include_files}

#     )
#     add_custom_target(
#             ros-echronos-doc-gen ALL
#             COMMAND doxygen Doxyfile.ros-echronos-docs
#             COMMAND cp -r html ../../../docs/doxygen
#             DEPENDS ros-echronos-docs
#     )
# endif()

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
            ${ROS_ECHRONOS_DIR}
            ${LIB_DIR}/tlsf #TODO: make this its own file
            ${echronos_build_dir}
            ${CMAKE_INCLUDE_PATH}
    )
    set_property(TARGET ${module_name} APPEND_STRING  PROPERTY COMPILE_FLAGS "-DROS_NODE_ID=${node_id} -DROS_INFO_SERIAL=${serial_on} ")
    add_dependencies(${module_name} ${echronos_target})
    
#    add_dependencies(ros_echronos_${module_name} ${ROS_BUILD_DIR})
endfunction()

set(MSG_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/msg_gen/cpp/")
function(generate_msgs ros_pkg msgs)
    foreach(msg ${msgs})
        add_custom_command(
                OUTPUT ${MSG_OUTPUT_DIR}/${ros_pkg}/${msg}.cpp ${MSG_OUTPUT_DIR}/include/${ros_pkg}/${msg}.hpp
                COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${ROS_ECHRONOS_DIR}/build_tools/genmsg_cpp.py ${ros_pkg} ${msg}
                COMMENT "Generating C++ code for msg ${ros_pkg}::${msgs}"
        )
#        add_custom_target(
#                ${ros_pkg}/${msg}
#                DEPENDS ${MSG_OUTPUT_DIR}/${ros_pkg}/${msg}.cpp
#        )
    endforeach()
endfunction()

function(depend_msgs target ros_pkg msgs)
    foreach(msg ${msgs})
        # add the source file to be compiled
        target_sources(
                ${target}
                PRIVATE
                    ${MSG_OUTPUT_DIR}/${ros_pkg}/${msg}.cpp
                PUBLIC
                    ${MSG_OUTPUT_DIR}/include/${ros_pkg}/${msg}.hpp
        )
        # update the include directories so the target can find its path
        target_include_directories(
                ${target}
                PUBLIC
                    ${MSG_OUTPUT_DIR}/include
        )
    endforeach()

endfunction()
