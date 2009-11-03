
FILE (GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

SET (full_sources
    ${sources}
    ${headers}
)

ADD_EXECUTABLE (tutorial ${full_sources})
TARGET_LINK_LIBRARIES (tutorial ${LIBS})