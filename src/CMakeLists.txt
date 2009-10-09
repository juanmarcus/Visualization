
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Build sub libraries
FOREACH (subdir ${VISIBI_LIBS})
    ADD_SUBDIRECTORY (${subdir})
ENDFOREACH (subdir ${VISIBI_LIBS})

# Build executables
ADD_SUBDIRECTORY (bin)