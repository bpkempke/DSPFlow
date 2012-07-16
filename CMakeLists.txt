#
# DSPFlow CMakeLists.txt
#
# Ben Kempke
#
# Shared libraries should be compiled as follows:
#  gcc -shared -nostartfiles -o bar bar.c
#
# Also make sure to install libdb in order to compile...:
#  sudo apt-get install libdb-dev

cmake_minimum_required(VERSION 2.6)
project(DSPFlow)

#Version 0.1
set(DSPFlow_VERSION_MAJOR 0)
set(DSPFlow_VERSION_MINOR 1)

#Set the paths to find the correct header files
include_directories(
	${CMAKE_SOURCE_DIR}/lib
)

#Find the threading library 'package'
find_package(Threads)

#And the library which provides dynamic linking (will be different for NaCl though...)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(DL)
if(NOT DL_FOUND)
	message(FATAL_ERROR "NO libdl... shutting down...")
endif()

add_subdirectory(src)
