#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

CMAKE_MINIMUM_REQUIRED( VERSION 3.0.0 )

PROJECT( gfx LANGUAGES CXX )

#
# CONFIGURATION
#

SET( CMAKE_VERBOSE_MAKEFILE on )

# Set compiler and linker flags
SET( CMAKE_CXX_STANDARD 11 )
SET( CMAKE_CXX_FLAGS "-Wall -fdiagnostics-color=always" )
SET( CMAKE_CXX_FLAGS_DEBUG   "-Og -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "-O3 -g0" )
SET( CMAKE_EXE_LINKER_FLAGS_DEBUG   "" )
SET( CMAKE_EXE_LINKER_FLAGS_RELEASE "-s" )

# set binary output directory
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib )

# Find necessary packages
FIND_PACKAGE( Threads REQUIRED )
FIND_PACKAGE( Boost 1.65 REQUIRED COMPONENTS system )
# yields ${Boost_LIBRARIES}

#
# LIBRARIES
#

# display libraries
file( GLOB GFX_SRC "*.cpp" )
file( GLOB GFX_FONT_SRC "*.cpp" )
ADD_LIBRARY( gfx ${GFX_SRC} ${GFX_FONT_SRC} )
TARGET_LINK_LIBRARIES( gfx mraa ${CMAKE_THREAD_LIBS_INIT} )
#TARGET_INCLUDE_DIRECTORIES( gfx PUBLIC some_dir )

#
# EXECUTABLES
#

file( GLOB TEST1_SRC "test1/*.cpp" )
ADD_EXECUTABLE( test1 ${TEST1_SRC} )
TARGET_LINK_LIBRARIES( test1 gfx )

file( GLOB TEST2_SRC "test2/*.cpp" )
ADD_EXECUTABLE( test2 ${TEST2_SRC} )
TARGET_LINK_LIBRARIES( test2 gfx )

file( GLOB TEST3_SRC "test3/*.cpp" )
ADD_EXECUTABLE( test3 ${TEST3_SRC} )
TARGET_LINK_LIBRARIES( test3 gfx )







