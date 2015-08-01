#
#  File    : CMakeLists.txt
#  Author  : YW. Jang
#  Date    : 2015.08.01.Sat
#
#  Copyright 2015, YW. Jang, All rights reserved.
#

cmake_minimum_required (VERSION 3.2)

project (unittest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # using gcc
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
    message(FATAL_ERROR "GCC version must be at least 4.8!")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -Wall")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # using Visual Studio C++
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

#set (EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR}/bin CACHE PATH "Output directory for applications" )

# Set compiler flags and options. 
# Here it is setting the Visual Studio warning level to 4
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
#message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

# The version number.
set (UNITTEST_VERSION_MAJOR 1)
set (UNITTEST_VERSION_MINOR 0)
set (UNITTEST_VERSION_REVISION 0)

set (PACKAGE_VERSION "${UNITTEST_VERSION_MAJOR}.${UNITTEST_VERSION_MINOR}.${UNITTEST_VERSION_REVISION}")

# Make sure the compiler can find include files from our libraries.
include_directories (${CMAKE_SOURCE_DIR}/garnut/include)

# Make sure the linker can find the garnut library once it is built.
#link_directories (${GARNUT_BINARY_DIR})

# Add executable called "unittest" that is built from the source files.
add_executable(unittest unittest.cpp)

set (ALL_LIBS
	 libgarnut)

target_link_libraries (unittest ${ALL_LIBS})