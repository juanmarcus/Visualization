
# Look for headers in ibi source dir
INCLUDE_DIRECTORIES(
    "${CMAKE_CURRENT_SOURCE_DIR}/vis"
)

# Build visualization library
ADD_SUBDIRECTORY (vis)

# Add our library to list of libraries
SET (LIBS ${LIBS} vis)

# Build executables
ADD_SUBDIRECTORY (bin)