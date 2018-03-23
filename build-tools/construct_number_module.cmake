include(echronos.cmake)
include(../libs/ros_echronos/CMakeLists.txt)
set(PRJ_TOOL enchronos/bin/prj.sh)

function(construct_numbat_module module_dir module_name)
    build_echronos_base(${module_name})
    build_ros_echronos(${module_dir} ${module_name})
endfunction()