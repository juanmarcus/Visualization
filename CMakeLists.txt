cmake_minimum_required (VERSION 2.6) 
project (VISUALIZATION)

#extra cmake stuff
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#uninstall target
CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET (uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

# Verbose makefile
SET (CMAKE_VERBOSE_MAKEFILE ON)

# Fix Teem_DIR
SET (Teem_DIR "/usr/local/lib" CACHE STRING "Directory with TeemConfig.cmake")

#find required packages
find_package (Teem REQUIRED)
find_package (Qt4 REQUIRED)
find_package (Boost 1.39.0 COMPONENTS program_options REQUIRED)

# Use files
include (${Teem_USE_FILE})
#set(QT_USE_QTOPENGL TRUE)
#set(QT_USE_QTXML TRUE)
include (${QT_USE_FILE})

# Include directories
#include_directories(
#    ${Boost_INCLUDE_DIR}
#)

#libs
set(LIBS ${LIBS} ${Teem_LIBRARIES} ${QT_LIBRARIES} ${Boost_LIBRARIES})

#definitions
#add_definitions(${QGLVIEWER_DEFINITIONS})

# List sub libraries
SET (VISIBI_LIBS
    widget
    cmdline
    error
)

SET (LIBS ${LIBS} ${VISIBI_LIBS})

# Dispatch the build into source subdirectory
ADD_SUBDIRECTORY (src)