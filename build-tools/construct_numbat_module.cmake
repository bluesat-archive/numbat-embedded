include(${BUILD_TOOLS_DIR}/echronos.cmake)
include(../../libs/ros_echronos/ros_echronos.cmake)
set(PRJ_TOOL enchronos/bin/prj.sh)

function(construct_numbat_module module_dir module_name)
    build_echronos_module(${module_name})
endfunction()