cmake_minimum_required (VERSION 2.6)
include (../CMakeCommon.txt)
project (fps_demo)

find_package (OpenGL)
find_package (GLUT)
find_package (OpenAL)

include_directories ( "${PROJECT_SOURCE_DIR}/../../build/include" ) 
link_directories ( "${PROJECT_SOURCE_DIR}/../../build/lib" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})

add_executable(fps_demo main.cpp Grenade.h SoundKit.h Weapon.h)

target_link_libraries(fps_demo ${CUBICVR_LIBRARIES} ${OPENAL_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

configure_file(${PROJECT_SOURCE_DIR}/../resources/images/particle_alpha.png ${PROJECT_BINARY_DIR}/particle_alpha.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/241-diffuse.png ${PROJECT_BINARY_DIR}/241-diffuse.png COPYONLY)
configure_files(${PROJECT_SOURCE_DIR}/../../cubicvr/shaders ${PROJECT_BINARY_DIR}/shaders)
configure_files(${PROJECT_SOURCE_DIR}/../resources/fps_demo/ ${PROJECT_BINARY_DIR}/)
configure_files(${PROJECT_SOURCE_DIR}/../resources/fps_demo/akumaq3dm8/ ${PROJECT_BINARY_DIR}/akumaq3dm8)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/fps_demo/akumaq3dm8/textures ${PROJECT_BINARY_DIR}/textures)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/fps_demo/akumaq3dm8/models ${PROJECT_BINARY_DIR}/models)
configure_files(${PROJECT_SOURCE_DIR}/../resources/fps_demo/audio ${PROJECT_BINARY_DIR}/audio)

