set(PRJ_TOOL ${BUILD_TOOLS_DIR}/echronos/bin/prj.sh)

function(clean_echronos echronos_build)
    execute_process(rm -rf out)
    execute_process(rm -rf ${echronos_build})
endfunction()

function(build_echronos_module module_name)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-bin.a
            MAIN_DEPENDECY ${module_name}.prx
            COMMENT "Building echronos for ${module_name}"
            COMMAND ls
            COMMAND ${PRJ_TOOL} --no-project --search-path ${BUILD_TOOLS_DIR}/echronos/share/packages --search-path ${CMAKE_CURRENT_SOURCE_DIR} --output . build ${module_name}
            COMMAND cp system.a ${module_name}-echronos.a
            COMMAND mkdir -p include
            COMMAND cp rtos-kochab.h include/
    )
    add_custom_target(
            ${module_name}-echronos.a
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${module_name}-bin.a
    )
    include_directories(
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    )
    message(STATUS "${module_name}-echronos.a")

endfunction()