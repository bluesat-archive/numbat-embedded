include(${BUILD_TOOLS_DIR}/build_env.cmake)
include(${BUILD_TOOLS_DIR}/construct_numbat_module.cmake)
include(${BUILD_TOOLS_DIR}/echronos.cmake)

set(BOILERPLATE_DIR ../boilerplate)

set(MODULE_DEP_FILES
    ${BOILERPLATE_DIR}/boilerplate.c
)
# NOTE: we don't include crtn or crti in the above because order is important


function(add_module module_name node_id serial serial_buffered files)
    add_library(${module_name}_crti STATIC ${BOILERPLATE_DIR}/crti.cpp)
    include_directories(
            ../boilerplate
            ${ROS_ECHRONOS_DIR}/include
    )
    construct_numbat_module(./ ${module_name})
    add_executable(${module_name} ${BOILERPLATE_DIR}/crtn.cpp ${files} ${MODULE_DEP_FILES} )
    build_ros_echronos(./ ${module_name} ${module_name}-echronos.a ${node_id} ${serial})
    target_link_libraries(${module_name} ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-echronos.a)
    target_link_libraries(${module_name} tlsf)
    
    if(${serial_buffered})
        message(STATUS "serial buffered")
        set_property(TARGET ${module_name} APPEND_STRING PROPERTY COMPILE_FLAGS "-DUART_BUFFERED ")
        target_link_libraries(${module_name} utils_buffered)
    else()
        message(STATUS "serial unbuffered")
        target_link_libraries(${module_name} utils)
    endif()
#    target_link_libraries(${module_name} driverlib utils ros_echronos_${module_name} tlsf ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-echronos.a)
    target_link_libraries(${module_name} driverlib)
    target_link_libraries(${module_name} ${LIBGCC})
    target_link_libraries(${module_name} ${LIBC})
    target_link_libraries(${module_name} ${module_name}_crti)
#    target_link_libraries(${module_name} m)
    add_custom_target(
            ${module_name}.elf ALL
            DEPENDS ${module_name}
            COMMENT "Copying elf for ${module_name}"
            COMMAND ls
            COMMAND mkdir -p ${CATKIN_DEVEL_PREFIX}/../elfs
            COMMAND cp -f ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION}/${module_name} ${CATKIN_DEVEL_PREFIX}/../elfs/${module_name}.elf
    )
    install(TARGETS ${module_name}
            RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    )
    if(${PART} MATCHES "TM4C123GH6PGE")
	message(AUTHOR_WARNING "\n\n* COMPILING FOR GENERIC PCB! *\n\n")
    elseif(${PART} MATCHES "TM4C123GH6PM")
	message(AUTHOR_WARNING "\n\n* COMPILING FOR TIVA BOARD! *\n\n")
    else()
	message(FATAL_ERROR "\n\n***********\nPART NOT DEFINED!")
    endif()
    

endfunction()
