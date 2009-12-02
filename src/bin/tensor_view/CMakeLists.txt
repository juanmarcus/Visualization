
FILE (GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

SET (moc_headers
    Viewer.h
)

QT4_WRAP_CPP (moc_sources ${moc_headers})

SET (full_sources
    ${sources}
    ${headers}
    ${moc_sources}
)

ADD_EXECUTABLE (tensor_view ${full_sources})
TARGET_LINK_LIBRARIES (tensor_view ${LIBS})