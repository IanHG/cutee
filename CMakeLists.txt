# Set CMake minimum version
cmake_minimum_required(VERSION 3.9)

# Describe project
project(utest VERSION 1.0.0 DESCRIPTION "Minimal Unit Testing library.")
set (CMAKE_CXX_STANDARD 14)

# Add
AUX_SOURCE_DIRECTORY(src source_code)

# Add library
add_library(utest SHARED
   ${source_code}
)

set_target_properties(utest PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(utest PROPERTIES SOVERSION 1)

set_target_properties(utest PROPERTIES PUBLIC_HEADER include/unit_test.h)

target_include_directories(utest PRIVATE .)

# Setup install directories
include(GNUInstallDirs)

install(TARGETS utest
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
