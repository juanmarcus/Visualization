# c executables
FILE (GLOB c_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

FOREACH (c_source ${c_sources})
    get_filename_component (target ${c_source} NAME_WE)
    add_executable (${target} ${c_source})
    target_link_libraries (${target} ${LIBS})
ENDFOREACH (c_source)

# c++ executables
FILE (GLOB cpp_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

FOREACH (cpp_source ${cpp_sources})
    get_filename_component (target ${cpp_source} NAME_WE)
    add_executable (${target} ${cpp_source})
    target_link_libraries (${target} ${LIBS})
ENDFOREACH (cpp_source)