
FILE (GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

SET (full_sources
    ${sources}
    ${headers}
    ${moc_sources}
)

ADD_EXECUTABLE (txf_editor ${full_sources})
TARGET_LINK_LIBRARIES (txf_editor ${LIBS})