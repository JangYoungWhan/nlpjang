#
#  File    : CMakeLists.txt
#  Author  : YW. Jang
#  Date    : 2015.08.01.Sat
#
#  Copyright 2015, YW. Jang, All rights reserved.
#

cmake_minimum_required (VERSION 3.2)

project (nlpjang)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Output directory for applications" )
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Output directory for libraries" )

# The name of garnut originated from garnet that known as the birthstone for January.
# It consists of a number of classes that use the base NLP library to provide basic utilities.
add_subdirectory (garnut)
#set (GARNUT_PROJECT_DIR garnut)

# The name of amothyst originated from amethyst that known as the birthstone for Febrary.
add_subdirectory (amuthyst)

# The name of aquamaron originated from aquamarin that known as the birthstone for March.
add_subdirectory (aquamaron)

# It is used for unittest all libraries.
add_subdirectory (unittest)