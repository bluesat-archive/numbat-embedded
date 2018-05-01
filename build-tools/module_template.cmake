include(${BUILD_TOOLS_DIR}/build_env.cmake)
include(${BUILD_TOOLS_DIR}/construct_numbat_module.cmake)
include(${BUILD_TOOLS_DIR}/echronos.cmake)

set(BOILERPLATE_DIR ../boilerplate)

set(MODULE_DEP_FILES
        ${BOILERPLATE_DIR}/boilerplate.c
        ${BOILERPLATE_DIR}/crti.cpp
        ${BOILERPLATE_DIR}/crtn.cpp
)


function(add_module module_name files)
    include_directories(
            ../boilerplate
    )
    construct_numbat_module(./ ${module_name})
    add_executable(${module_name} ${files} ${MODULE_DEP_FILES})
    target_link_libraries(${module_name} driverlib utils ros_echronos_${module_name} tlsf ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-echronos.a)
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