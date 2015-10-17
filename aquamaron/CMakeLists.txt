#
#  File    : CMakeLists.txt
#  Author  : YW. Jang
#  Date    : 2015.08.26.Tue
#
#  Copyright 2015, YW. Jang, All rights reserved.
#

cmake_minimum_required (VERSION 2.8)

project (libaquamaron)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # using gcc
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
    message(FATAL_ERROR "GCC version must be at least 4.8!")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -Wall")
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") # using Visual Studio C++
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
    message(FATAL_ERROR "MSVC version must be at least VC110!")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc")
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # using Clang
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif()
else()
  message (FATAL_ERROR "Did not support this platform!")
endif()

add_definitions(-DUNICODE -D_UNICODE)

# The version number.
set (AQUAMARON_VERSION_MAJOR 1)
set (AQUAMARON_VERSION_MINOR 0)
set (AQUAMARON_VERSION_REVISION 0)

set (PACKAGE_VERSION "${AQUAMARON_VERSION_MAJOR}.${AQUAMARON_VERSION_MINOR}.${AQUAMARON_VERSION_REVISION}")

# Make sure the compiler can find include files from our libraries.
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/include)
#include_directories (${CMAKE_SOURCE_DIR}/garnut/include)

file (GLOB_RECURSE LIB_HDRS *.hpp)
file (GLOB_RECURSE LIB_SRCS *.cpp)

add_library (libaquamaron STATIC
             ${LIB_HDRS}
             ${LIB_SRCS})

# Set dependent libraries.
set (ALL_LIBS
   libgarnut)
target_link_libraries (libaquamaron ${ALL_LIBS})