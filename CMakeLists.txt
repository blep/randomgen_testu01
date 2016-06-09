cmake_minimum_required(VERSION 2.8)

PROJECT(PCG_c)
include(CheckCSourceCompiles) 
INCLUDE(ExternalProject)

# Output all libraries and executables in the same directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin
    CACHE PATH "Single directory for all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin
    CACHE PATH "Single directory for all executables.")
mark_as_advanced( LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH )

macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

macro(use_cpp14)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    endif ()
  else ()
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED on)
  endif ()
endmacro(use_cpp14)

use_c99()
use_cpp14()


set(THIRDPARTIES_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparties)

externalproject_add(
    testu01_prj
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparties/TestU01
    CONFIGURE_COMMAND ./configure --prefix=${THIRDPARTIES_INSTALL_DIR} --disable-shared
    BUILD_COMMAND make install
    INSTALL_DIR ${THIRDPARTIES_INSTALL_DIR}
    BUILD_IN_SOURCE 1
)
set(THIRDPARTIES_LIB_DIR ${THIRDPARTIES_INSTALL_DIR}/lib)
set(TESTU01_LIBRARIES 
    ${THIRDPARTIES_LIB_DIR}/libtestu01.a 
    ${THIRDPARTIES_LIB_DIR}/libmylib.a 
    ${THIRDPARTIES_LIB_DIR}/libprobdist.a)

# Adds third-parties include/lib dir to look-up paths
include_directories(BEFORE ${THIRDPARTIES_INSTALL_DIR}/include)
link_directories(${THIRDPARTIES_INSTALL_DIR}/lib)
include_directories(thirdparties/pcg-c/include)
set(SMHASHER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparties/smhasher/src)

option(PCG_C_BUILD_SAMPLES "Build pgc-c samples" OFF)
option(PCG_C_BUILD_TESTS "Build pgc-c tests" OFF)

add_subdirectory( thirdparties/pcg-c/ )
add_subdirectory( src/ )
