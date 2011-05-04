cmake_minimum_required (VERSION 2.6)
include (../CMakeCommon.txt)
project (vehicle_physics)

find_package (OpenGL)
find_package (GLUT)

if(WIN32)
configure_file(${PROJECT_SOURCE_DIR}/../resources/win32/GLUT/glut32.dll ${PROJECT_BINARY_DIR}/glut32.dll COPYONLY)
endif(WIN32)

include_directories ( "${PROJECT_SOURCE_DIR}/../../build/include" ) 
link_directories ( "${PROJECT_SOURCE_DIR}/../../build/lib" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})

add_executable(vehicle_physics main.cpp)

target_link_libraries(vehicle_physics ${CUBICVR_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

configure_files(${PROJECT_SOURCE_DIR}/../../cubicvr/shaders ${PROJECT_BINARY_DIR}/shaders)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/particle_alpha.png ${PROJECT_BINARY_DIR}/particle_alpha.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/models/box2.lwo ${PROJECT_BINARY_DIR}/box2.lwo COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/models/skysphere.lwo ${PROJECT_BINARY_DIR}/skysphere.lwo COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/sky.png ${PROJECT_BINARY_DIR}/sky.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/1384-diffuse.jpg ${PROJECT_BINARY_DIR}/1384-diffuse.jpg COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/grass.png ${PROJECT_BINARY_DIR}/grass.png COPYONLY)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/car_default/models ${PROJECT_BINARY_DIR}/)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/car_default/textures ${PROJECT_BINARY_DIR}/)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/car_track/models ${PROJECT_BINARY_DIR}/)
configure_files_recurse(${PROJECT_SOURCE_DIR}/../resources/car_track/textures ${PROJECT_BINARY_DIR}/)


