#-----------------------------------------------------------------------------
# Project configuration
cmake_minimum_required (VERSION 2.6) 
project (VISUALIZATION)

#-----------------------------------------------------------------------------
# Extra cmake stuff
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Uninstall target
CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/CMake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET (uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

# Verbose makefile
SET (CMAKE_VERBOSE_MAKEFILE ON)

#-----------------------------------------------------------------------------
# Find required packages
find_package (IBI REQUIRED)
find_package (Teem REQUIRED)
find_package (Qt4 REQUIRED)
find_package (QGLViewer REQUIRED)

#-----------------------------------------------------------------------------
# Use files
include (${IBI_USE_FILE})
include (${Teem_USE_FILE})
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
include (${QT_USE_FILE})

#-----------------------------------------------------------------------------
# Include directories
INCLUDE_DIRECTORIES (
    ${QGLVIEWER_INCLUDE_DIR}
)

#-----------------------------------------------------------------------------
# Definitions
ADD_DEFINITIONS(
    ${QGLVIEWER_DEFINITIONS}
)

#-----------------------------------------------------------------------------
# Configure libraries 
set(LIBS ${LIBS} ${IBI_LIBRARIES} ${Teem_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARY})

#-----------------------------------------------------------------------------
# Dispatch the build into source subdirectory
ADD_SUBDIRECTORY (src)