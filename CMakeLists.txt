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
#SET (CMAKE_VERBOSE_MAKEFILE ON)

#find required packages
find_package (Teem REQUIRED)

#use files
include(${Teem_USE_FILE})

#includes
#include_directories(
#)

#libs
set (LIBS ${LIBS} ${Teem_LIBRARIES}) 

#definitions
#add_definitions(${QGLVIEWER_DEFINITIONS})

#parse subdirectories
add_subdirectory (src)
