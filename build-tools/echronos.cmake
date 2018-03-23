set(PRJ_TOOL echronos/bin/prj.sh)

function(clean_echronos echronos_build)
    execute_process(rm -rf out)
    execute_process(rm -rf ${echronos_build})
endfunction()

function(build_echronos_module module_name)
    add_custom_command(
            OUTPUT ${LIB_BUILD}/${module_name}-echronos.a
            COMMAND ${PRJ_TOOL} --no-project --search-path echronos/share/packages --search-path . --output ${ECHRONOS_BUILD}/${module_name} build ${module_name}
            COMMAND cp ${ECHRONOS_BUILD}/${module_name}/system.a ${LIB_BUILD}/${module-name}-echronos.a
            COMMAND rm -r out
            MAIN_DEPDENCY ${MODULES_DIR}/${module_name}.prx
    )
endfunction()