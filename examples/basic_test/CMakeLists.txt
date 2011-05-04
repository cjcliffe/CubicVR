cmake_minimum_required (VERSION 2.6)
include (../CMakeCommon.txt)
project (basic_test)

if(WIN32)
configure_file(${PROJECT_SOURCE_DIR}/../resources/win32/GLUT/glut32.dll ${PROJECT_BINARY_DIR}/glut32.dll COPYONLY)
endif(WIN32)

find_package (OpenGL)
find_package (GLUT)

include_directories ( "${PROJECT_SOURCE_DIR}/../../build/include" ) 
link_directories ( "${PROJECT_SOURCE_DIR}/../../build/lib" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR})
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})

add_executable(basic_test main.cpp)

target_link_libraries(basic_test ${CUBICVR_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

configure_file(${PROJECT_SOURCE_DIR}/../resources/images/panel-diff.png ${PROJECT_BINARY_DIR}/panel-diff.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/panel-spec.png ${PROJECT_BINARY_DIR}/panel-spec.png COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/../resources/images/panel-norm.png ${PROJECT_BINARY_DIR}/panel-norm.png COPYONLY)
configure_files(${PROJECT_SOURCE_DIR}/../../cubicvr/shaders ${PROJECT_BINARY_DIR}/shaders)

