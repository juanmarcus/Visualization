FILE (GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

FOREACH (source ${sources})
    get_filename_component(target ${source} NAME_WE)
    add_executable (${target} ${source})
    target_link_libraries (${target} ${LIBS})
ENDFOREACH(source)