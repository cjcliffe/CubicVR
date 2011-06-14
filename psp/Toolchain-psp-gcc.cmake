# You will need at least CMake 2.6.0.
#
# Adjust the following paths to suit your environment.
#
# This file was based on http://www.cmake.org/Wiki/CmakeMingw

# The name of the target operating system.
set(CMAKE_SYSTEM_NAME Generic)

# Location of target environment.
find_program(psp-config_SCRIPT psp-config)
if (psp-config_SCRIPT)
    execute_process(COMMAND ${psp-config_SCRIPT}
                    ARGS --psp-prefix
                    OUTPUT_VARIABLE PSP_PREFIX
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${psp-config_SCRIPT}
                    ARGS --pspsdk-path
                    OUTPUT_VARIABLE PSPSDK_PATH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
else (psp-config_SCRIPT)
    message(FATAL_ERROR "psp-config was not found.\nInstall the PSPDEV toolchain or update the $PATH environment variable.")
endif (psp-config_SCRIPT)

set(CMAKE_SYSTEM_INCLUDE_PATH "${PSP_PREFIX}/include")
set(CMAKE_SYSTEM_LIBRARY_PATH "${PSP_PREFIX}/lib")
set(CMAKE_SYSTEM_PROGRAM_PATH "${PSP_PREFIX}/bin")

# Which compilers to use for C and C++.
set(CMAKE_C_COMPILER psp-gcc)
set(CMAKE_CXX_COMPILER psp-g++)

# Needed to pass the compiler tests.
set(LINK_DIRECTORIES ${PSPSDK_PATH}/lib)
set(LINK_LIBRARIES -lc -lpspuser -lpspkernel -lc)

# Adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment.
set(CMAKE_FIND_ROOT_PATH ${PSP_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# After building the ELF binary build the PSP executable.
function(add_psp_executable nm)
    get_target_property(PSP_EXECUTABLE_OUTPUT_NAME ${nm} OUTPUT_NAME)
    if (NOT PSP_EXECUTABLE_OUTPUT_NAME)
        set(PSP_EXECUTABLE_OUTPUT_NAME ${nm})
    endif(NOT PSP_EXECUTABLE_OUTPUT_NAME)
    set_target_properties(
        ${nm}
        PROPERTIES LINK_FLAGS "-specs=${PSPSDK_PATH}/lib/prxspecs -Wl,-q,-T${PSPSDK_PATH}/lib/linkfile.prx ${PSPSDK_PATH}/lib/prxexports.o -L${PSPSDK_PATH}/lib"
        )
    add_custom_command(
        TARGET ${nm}
        POST_BUILD 
        COMMAND psp-fixup-imports ${PSP_EXECUTABLE_OUTPUT_NAME}
        COMMAND mksfo '${PSP_EBOOT_TITLE}' PARAM.SFO
        COMMAND psp-prxgen ${PSP_EXECUTABLE_OUTPUT_NAME} ${PSP_EXECUTABLE_OUTPUT_NAME}.prx
        COMMAND pack-pbp EBOOT.PBP PARAM.SFO ${PSP_EBOOT_ICON} NULL NULL ${PSP_EBOOT_PIC1} NULL ${PSP_EXECUTABLE_OUTPUT_NAME}.prx NULL
    )
endfunction()

set(PSP 1)
