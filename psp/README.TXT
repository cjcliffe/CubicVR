Modified build to use CMake on the PSP

usage:

# build cubicvr lib
mkdir cmake
cd cmake
cmake -DCMAKE_TOOLCHAIN_FILE=../psp/Toolchain-psp-gcc.cmake ..
make

# build example
cd psp/examples/metacube_lws
mkdir cmake
cd cmake
cmake -DCMAKE_TOOLCHAIN_FILE=../../../Toolchain-psp-gcc.cmake ..
make
