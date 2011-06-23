cmake_minimum_required (VERSION 2.6)
project (CubicVR)

SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build/")
if(NOT PSP)
find_package (OpenGL)
endif(NOT PSP)

# add_definitions (-DUSE_LIBJPEG)

add_subdirectory (cubicvr/external)
add_subdirectory (cubicvr)

