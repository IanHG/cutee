################################################################################
#
# Header
#
################################################################################
# Set CMake minimum version
cmake_minimum_required(VERSION 3.9)

# Describe project
project(utest VERSION 1.0.0 DESCRIPTION "Minimal Unit Testing library.")
set (CMAKE_CXX_STANDARD 14)

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
add_library(utest SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(utest PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(utest PROPERTIES SOVERSION 1)
set_target_properties(utest PROPERTIES PUBLIC_HEADER include/unit_test.h)

# Add static library 
add_library(utest_static STATIC $<TARGET_OBJECTS:objlib>)
set_target_properties(utest_static PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(utest_static PROPERTIES SOVERSION 1)
set_target_properties(utest_static PROPERTIES PUBLIC_HEADER include/unit_test.h)

################################################################################
#
# Install setup
#
################################################################################
# Install part
include(GNUInstallDirs)

install(TARGETS utest
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS utest_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
