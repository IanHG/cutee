################################################################################
#
# Header
#
################################################################################
# Set CMake minimum version
cmake_minimum_required(VERSION 3.9)

# Describe project
project(cutee VERSION 2.0.4 DESCRIPTION "Minimal Unit Testing library.")
set (CMAKE_CXX_STANDARD 17)

#-----------------------------------------------------------------------------
# Enable ccache if not already enabled by symlink masquerading and if no other
# CMake compiler launchers are already defined
#-----------------------------------------------------------------------------
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if(CCACHE_EXECUTABLE)
  foreach(LANG C CXX)
    if(NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
      message(STATUS "Enabling ccache for ${LANG}")
      set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
    endif()
  endforeach()
endif()

################################################################################
#
# Build libraries
#
################################################################################
# Create a variable with all source files files
AUX_SOURCE_DIRECTORY(src libsrc)

# This is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})
target_include_directories(objlib PRIVATE .) # Set main dir as search path

# Shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# Add dynamic library
add_library(cutee SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(cutee PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cutee PROPERTIES SOVERSION 1)
set_target_properties(cutee PROPERTIES PUBLIC_HEADER 
"\
include/cutee/asserter.hpp;\
include/cutee/assertion.hpp;\
include/cutee/collection.hpp;\
include/cutee/container.hpp;\
include/cutee/exceptions.hpp;\
include/cutee/float_eq.hpp;\
include/cutee/formater.hpp;\
include/cutee/function.hpp;\
include/cutee/macros.hpp;\
include/cutee/message.hpp;\
include/cutee/meta.hpp;\
include/cutee/osutil.hpp;\
include/cutee/performance_test.hpp;\
include/cutee/suite.hpp;\
include/cutee/test.hpp;\
include/cutee/timer.hpp;\
include/cutee/typedef.hpp;\
include/cutee/version.hpp;\
include/cutee/writer.hpp;\
")

# Add static library 
add_library(cutee_static STATIC $<TARGET_OBJECTS:objlib>)
set_target_properties(cutee_static PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cutee_static PROPERTIES SOVERSION 1)
#set_target_properties(cutee_static PROPERTIES PUBLIC_HEADER include/unit_test.hpp)

################################################################################
#
# Install setup
#
################################################################################
# Install part
include(GNUInstallDirs)

install(TARGETS cutee
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cutee
    )

install(FILES
   include/cutee.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cutee
   )

install(TARGETS cutee_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cutee
    )
