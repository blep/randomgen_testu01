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

use_c99()

set(THIRDPARTIES_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparties)

externalproject_add(
	testu01
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparties/TestU01
	CONFIGURE_COMMAND ./configure --prefix=${THIRDPARTIES_INSTALL_DIR}
	INSTALL_DIR ${THIRDPARTIES_INSTALL_DIR}
	BUILD_IN_SOURCE 1
)

include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_subdirectory( src/ )

