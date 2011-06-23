# Project name
cmake_minimum_required (VERSION 2.6) 
project (metacube_lws)

set (CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti")
add_definitions(-G0 -DARCH_PSP)

include_directories(SYSTEM ${PSPSDK_PATH}/include)
link_directories(${PSPSDK_PATH}/lib ${CMAKE_SOURCE_DIR}/../../../build/lib)

set(PSP_EBOOT_TITLE "CubicVR - Metacube w Lighting")
set(PSP_EBOOT_ICON "${CMAKE_SOURCE_DIR}/ICON0.PNG")
set(PSP_EBOOT_PIC1 "${CMAKE_SOURCE_DIR}/PIC1.PNG")

set(SRC_LIST
	main.cpp
	Gamepad.cpp)
    
include_directories(${CMAKE_SOURCE_DIR}/../../../build/include)

# list of required libs
set(LIB_LIST
	c
	m
	cubicvr
	bulletdynamics
	bulletcollision
	bulletlinearmath
	stdc++)

# binary will be called crenderer
add_executable (metacube_lws.elf ${SRC_LIST})
target_link_libraries(metacube_lws.elf ${LIB_LIST} pspgum pspgu pspaudiolib psppower pspaudio pspgu psprtc pspdebug pspdisplay pspge pspctrl pspsdk c pspnet pspnet_inet pspnet_resolver psputility pspuser pspkernel)
add_psp_executable(metacube_lws.elf)
