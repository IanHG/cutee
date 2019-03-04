################################################################################
#
# Header
#
################################################################################
# Set CMake minimum version
cmake_minimum_required(VERSION 3.9)

# Describe project
project(cutee VERSION 1.0.0 DESCRIPTION "Minimal Unit Testing library.")
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
add_library(cutee SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(cutee PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cutee PROPERTIES SOVERSION 1)
set_target_properties(cutee PROPERTIES PUBLIC_HEADER 
"\
include/failed_data.hpp;\
include/float_eq.hpp;\
include/performance_test.hpp;\
include/collection.hpp;\
include/test_failed.hpp;\
include/test_suite.hpp;\
include/timer.hpp;\
include/unit_assert.hpp;\
include/unit_test_factory.hpp;\
include/unit_test.hpp\
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
   include/cutee.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )

install(TARGETS cutee_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cutee
    )
