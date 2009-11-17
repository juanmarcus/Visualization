
# Parse sub dirs for sources
FILE (GLOB_RECURSE sources "*.cpp")
FILE (GLOB_RECURSE headers "*.h")

SET (moc_headers
    NrrdVolumeViewer.h
    TransferFunctionEditor.h
    RaycastingViewer.h
)

QT4_WRAP_CPP (moc_sources ${moc_headers})

SET (full_sources
    ${sources}
    ${headers}
    ${moc_sources}
)

# Compile main ibi library
ADD_LIBRARY (vis SHARED ${full_sources})
TARGET_LINK_LIBRARIES (vis ${LIBS})