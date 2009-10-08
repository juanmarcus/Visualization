FILE (GLOB c_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

FOREACH (source ${c_sources})
    get_filename_component (target ${source} NAME_WE)
    add_executable (${target} ${source})
    target_link_libraries (${target} ${LIBS})
ENDFOREACH (source)