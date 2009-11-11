
FILE (GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

SET (moc_headers
    MainWindow.h
)

QT4_WRAP_CPP (moc_sources ${moc_headers})

SET (full_sources
    ${sources}
    ${headers}
    ${moc_sources}
)

ADD_EXECUTABLE (workspace ${full_sources})
TARGET_LINK_LIBRARIES (workspace ${LIBS})