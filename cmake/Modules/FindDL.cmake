# - find where dlopen and friends are located.
# DL_FOUND - system has dynamic linking interface available
# DL_INCLUDE_DIR - where dlfcn.h is located.
# DL_LIBRARIES - libraries needed to use dlopen

##########
#OPTION 1#
##########
#include(CheckFunctionExists)
#
#find_path(DL_INCLUDE_DIR NAMES dlfcn.h)
#find_library(DL_LIBRARIES NAMES dl)
#if(DL_LIBRARIES)
#  set(DL_FOUND)
#else(DL_LIBRARIES)
#  check_function_exists(dlopen DL_FOUND)
#  # If dlopen can be found without linking in dl then dlopen is part
#  # of libc, so don't need to link extra libs.
#  set(DL_LIBRARIES "")
#endif(DL_LIBRARIES)

##########
#OPTION 2#
##########
#if(DL_INCLUDE_DIR)
#	  set(DL_FIND_QUIETLY TRUE)
#endif()

find_path(DL_INCLUDE_DIR dlfcn.h)
find_library(DL_LIBRARY NAMES dl)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DL DEFAULT_MSG DL_LIBRARY DL_INCLUDE_DIR)

if(NOT DL_FOUND)
    # if dlopen can be found without linking in dl then,
    # dlopen is part of libc, so don't need to link extra libs.
    check_function_exists(dlopen DL_FOUND)
    set(DL_LIBRARY "")
endif()

set(DL_LIBRARIES ${DL_LIBRARY})

#mark_as_advanced(DL_LIBRARY DL_INCLUDE_DIR)

##########
#OPTION 3#
##########
#INCLUDE(CheckFunctionExists)
#	IF(DL_INCLUDE_DIR AND DL_LIBRARIES)
#		SET(DL_FOUND TRUE)
#	ELSE(DL_INCLUDE_DIR AND DL_LIBRARIES)
#		FIND_PATH(DL_INCLUDE_DIR NAMES dlfcn.h)
#		FIND_LIBRARY(DL_LIBRARIES NAMES dl)
#		IF(DL_LIBRARIES)
#			MESSAGE(STATUS "Found dlopen")
#					SET(DL_FOUND TRUE)
#		ELSE(DL_LIBRARIES)
#			check_function_exists(dlopen DL_FOUND)
#			# If dlopen can be found without linking in dl then dlopen is part
#			# of libc, so don't need to link extra libs.
#			SET(DL_LIBRARIES "")
#				ENDIF(DL_LIBRARIES)
#	ENDIF(DL_INCLUDE_DIR AND DL_LIBRARIES)
