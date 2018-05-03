include(${BUILD_TOOLS_DIR}/build_env.cmake)
include(${BUILD_TOOLS_DIR}/construct_numbat_module.cmake)
include(${BUILD_TOOLS_DIR}/echronos.cmake)

set(BOILERPLATE_DIR ../boilerplate)

set(MODULE_DEP_FILES
    ${BOILERPLATE_DIR}/boilerplate.c
)
# NOTE: we don't include crtn or crti in the above because order is important


function(add_module module_name files)
    include_directories(
            ../boilerplate
            ${ROS_ECHRONOS_DIR}/include
    )
    construct_numbat_module(./ ${module_name})
    add_executable(${module_name} ${BOILERPLATE_DIR}/crtn.cpp ${files} ${MODULE_DEP_FILES} ${BOILERPLATE_DIR}/crti.cpp ${ros_files})
    set_target_properties(${module_name} PROPERTIES COMPILE_FLAGS "-DROS_NODE_ID=1 -DROS_INFO_SERIAL=${serial_on}")
#    target_link_libraries(${module_name} driverlib utils ros_echronos_${module_name} tlsf ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-echronos.a)
    target_link_libraries(${module_name} driverlib utils tlsf ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-echronos.a)
    target_link_libraries(${module_name} ${LIBGCC})
    target_link_libraries(${module_name} ${LIBC})
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

endfunction()